#include "harvesthandler.h"
#include "harvesttask.h"
#include "harvestproject.h"
#include "task.h"
#include <QTextStream>
#include <QApplication>
#include <QTcpSocket>
#include <QDataStream>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include<QRegularExpression>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QDebug>
#include <QNetworkInformation>

HarvestHandler *HarvestHandler::harvest_handler{nullptr};
const QString HarvestHandler::default_grant_type{"authorization_code"};
const QString HarvestHandler::refresh_grant_type{"refresh_token"};
const int HarvestHandler::request_timeout_constant{10 * 1000};

HarvestHandler *HarvestHandler::get_instance(const QDir &config_dir) {
    if (harvest_handler == nullptr) {
        harvest_handler = new HarvestHandler(config_dir);
    }

    return harvest_handler;
}

void HarvestHandler::reset_instance() {
    delete harvest_handler;
    harvest_handler = nullptr;
}

HarvestHandler::HarvestHandler(const QDir &config_dir)
        : auth_server{nullptr}, auth_socket{nullptr}, auth_file(config_dir.absolutePath() + "/" + auth_file_name),
          settings_manager{SettingsManager::get_instance(config_dir)}, network_manager(this), loop(),
          is_network_reachable{true} {
    if (QNetworkInformation::instance() != nullptr) {
        this->set_network_reachability(QNetworkInformation::instance()->reachability());
    }

    network_manager.setAutoDeleteReplies(true);

    // try and load the authentication details stored in the user configuration directories
    this->json_auth = get_authentication();
    // this auth is considered found not only if we read it, but we also need to make sure all the necessary fields are present
    auth_found = !json_auth.isEmpty() && json_auth_is_complete();

    if (!auth_found) {
        login();
    } else {
        load_account_id();
        emit ready();
    }
}

HarvestHandler::~HarvestHandler() {
    SettingsManager::reset_instance();

    delete auth_server;
    delete auth_socket;

    // https://stackoverflow.com/questions/19157946/does-stdmap-destructor-call-key-destructors-as-well-as-value-destructors
}

// this is an additional check that validates whether there's at least one more day of validity in the auth
// token or we need to refresh it before any requests can be performed
void HarvestHandler::check_authenticate() {
    if (!json_auth_is_safely_active()) {
        QString refresh_token(json_auth["refresh_token"].toString());
        authenticate_request(nullptr, &refresh_token);
    }
}

QJsonDocument HarvestHandler::get_authentication() {
    if (!auth_file.exists()) {
        return {};
    }

    return get_auth_details();
}

void HarvestHandler::login() {
    QDesktopServices::openUrl(login_url);

    auth_server = new QTcpServer();

    if (!auth_server->listen(QHostAddress::Any, 23456)) {
        const QString error_string{
                QApplication::translate("HarvestHandler", "Error while waiting for Harvest authorization")};
        QMessageBox::information(nullptr, QApplication::translate("HarvestHandler", "Error"), error_string);
        QApplication::quit();
    }

    connect(auth_server, &QTcpServer::newConnection, this, &HarvestHandler::new_connection);
}

QJsonDocument HarvestHandler::get_auth_details() {
    auth_file.open(QFile::ReadOnly | QFile::Text);
    QTextStream text_stream(&auth_file);

    const QByteArray auth_text{text_stream.readAll().toUtf8()};
    QJsonDocument auth_details{QJsonDocument::fromJson(auth_text)};

    auth_file.close();

    return auth_details;
}

bool HarvestHandler::json_auth_is_complete() {
    const QJsonObject json_object{json_auth.object()};

    bool contains_access_token{json_object.contains("access_token")};
    bool contains_refresh_token{json_object.contains("refresh_token")};
    bool contains_token_type{json_object.contains("token_type")};
    bool contains_expires_in{json_object.contains("expires_in")};

    return contains_access_token && contains_refresh_token && contains_token_type && contains_expires_in;
}

bool HarvestHandler::json_auth_is_safely_active() {
    auto expires_on{QDateTime::fromMSecsSinceEpoch(json_auth["expires_on"].toInteger())};

    // if there's less than a full day of time left before the token expires...
    return expires_on > QDateTime::currentDateTime().addDays(1);
}

void HarvestHandler::new_connection() {
    auth_socket = auth_server->nextPendingConnection();

    connect(auth_socket, &QTcpSocket::readyRead, this, &HarvestHandler::code_received);
}

void HarvestHandler::code_received() {
    if (auth_socket->canReadLine()) {
        QStringList tokens = QString(auth_socket->readLine()).split(QRegularExpression("[ \r\n][ \r\n]*"));

        const QString message_string{QApplication::translate("HarvestHandler",
                                                             "Authentication successful, you may now close this tab")};
        const QString final_message = get_http_message(message_string);
        auth_socket->write(final_message.toUtf8());
        auth_socket->flush();
        auth_socket->close();
        auth_server->close();
        auth_socket->deleteLater();
        auth_server->deleteLater();

        for (QString &token: tokens) {
            if (token.contains("?")) {
                std::map<QString, QString> query_map{parse_query_string(token)};
                if (!query_map.contains("code") || !query_map.contains("scope")) {
                    QString error_string{
                            QApplication::translate("HarvestHandler",
                                                    "The details received from Harvest did not contain the minimum details required")};
                    QMessageBox::information(nullptr, QApplication::translate("HarvestHandler", "Incomplete Details"),
                                             error_string);
                    QApplication::quit();
                }
                authenticate_request(&query_map["code"], nullptr);
                get_user_details(query_map["scope"]);
            }
        }
    }
}

std::map<QString, QString> HarvestHandler::parse_query_string(QString &query_string) {
    // remove the starting "/?" from the query string
    query_string.remove(0, 2);


    QStringList values{query_string.split("&")};
    std::map<QString, QString> query_map;
    for (QString &value: values) {
        QStringList detailed_value{value.split("=")};
        query_map.emplace(detailed_value[0], detailed_value[1]);
    }

    return query_map;
}

void HarvestHandler::authentication_received(const QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        const QString error_string{
                QApplication::translate("HarvestHandler", "Error while authenticating with Harvest services: ") +
                reply->errorString()};
        QMessageBox::information(nullptr, QApplication::translate("HarvestHandler", "Error authenticating"),
                                 error_string);
        QApplication::quit();
        return;
    }

    // reply is originally non-const so this cast should be safe while allowing to use it
    // as const in the previous error handling
    json_auth = read_close_reply(const_cast<QNetworkReply *>(reply));
    QJsonObject json_object{json_auth.object()};
    qint64 seconds{json_object["expires_in"].toInteger()};
    json_object.insert("expires_on", QDateTime::currentDateTime().addSecs(seconds).toMSecsSinceEpoch());
    json_auth.setObject(json_object);

    save_authentication();
}

QJsonDocument HarvestHandler::read_close_reply(QNetworkReply *reply) {
    QByteArray response_body{static_cast<QString>(reply->readAll()).toUtf8()};
    reply->deleteLater();
    return QJsonDocument::fromJson(response_body);
}

void HarvestHandler::save_authentication() {
    if (!auth_file.open(QFile::WriteOnly | QFile::Text)) {
        QString error_text{QApplication::translate("HarvestHandler",
                                                   "It was not possible to save your credentials, you will have to log in again the next time you open Harvest Timer")};
        QMessageBox::information(nullptr, QApplication::translate("HarvestHandler", "Error saving authentication"),
                                 error_text);
        return;
    }

    auth_file.write(json_auth.toJson(QJsonDocument::Compact));

    auth_file.close();
}

void HarvestHandler::authenticate_request(QString *auth_code, QString *refresh_token) {
    if (!is_network_reachable) {
        QMessageBox::warning(nullptr, QApplication::translate("HarvestHandler", "Network Unreachable"),
                             QApplication::translate("HarvestHandler",
                                                     "You are currently not connected to the internet, please reconnect and try again"));
        return;
    }

    QUrl request_url(auth_url);
    QNetworkRequest request(request_url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Harvest Timer Qt (jorge_barroso_11@hotmail.com)");
    request.setRawHeader("Accept", "application/json");

    QUrlQuery url_query;
    url_query.addQueryItem("client_id", client_id);
    url_query.addQueryItem("client_secret", client_secret);

    QString grant_type;
    if (auth_code != nullptr) {
        url_query.addQueryItem("code", *auth_code);
        grant_type = HarvestHandler::default_grant_type;
    } else if (refresh_token != nullptr) {
        url_query.addQueryItem("refresh_token", *refresh_token);
        grant_type = HarvestHandler::refresh_grant_type;
    }
    url_query.addQueryItem("grant_type", grant_type);


    QNetworkReply *reply{network_manager.post(request, url_query.toString().toUtf8())};

    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    authentication_received(reply);
}


std::vector<HarvestProject> HarvestHandler::update_user_data() {
    if (!is_network_reachable) {
        QMessageBox::warning(nullptr,
                             QApplication::translate("HarvestHandler", "Network Unreachable"),
                             QApplication::translate("HarvestHandler",
                                                     "You are currently not connected to the internet, please reconnect and try again"));
        return projects;
    }

    QUrl request_url(assignments_url);

    int total_pages = -1;
    for (int page = 1; total_pages == -1 || page <= total_pages; ++page) {
        QUrlQuery url_query;
        url_query.addQueryItem("page", QString::number(page));
        url_query.addQueryItem("per_page", pagination_records);
        request_url.setQuery(url_query);

        QNetworkReply *reply{do_request_with_auth(request_url, true, "GET")};
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->error();
        }

        const QJsonDocument json_payload{read_close_reply(reply)};
        get_projects_data(json_payload, projects);

        if (total_pages == -1) {
            total_pages = json_payload["total_pages"].toInt();
        }
    }

    return projects;
}

void
HarvestHandler::get_projects_data(const QJsonDocument &json_payload, std::vector<HarvestProject> &projects_vector) {
    for (const QJsonValue project_assignment: json_payload["project_assignments"].toArray()) {
        const QString client_name{project_assignment["client"]["name"].toString()};
        std::vector<HarvestTask> project_tasks_vector;
        for (const QJsonValue json_task: project_assignment["task_assignments"].toArray()) {
            const QJsonValue json_task_details{json_task["task"]};
            const HarvestTask task = HarvestTask{
                    .task_id = json_task_details["id"].toInteger(),
                    .task_name = json_task_details["name"].toString(),
                    .client_name = client_name
            };

            project_tasks_vector.emplace_back(task);
        }

        const HarvestProject project{
                project_assignment["client"]["name"].toString(),
                project_assignment["project"]["name"].toString(),
                project_assignment["project"]["id"].toInteger(),
                project_tasks_vector
        };
        projects_vector.emplace_back(project);
    }
}

void HarvestHandler::get_user_details(const QString &scope) {
    account_id = scope.split("%3A")[1];
    settings_manager->add_setting(account_id_key, account_id);

    user_id = get_user_id();
    settings_manager->add_setting(user_id_key, user_id);

    emit ready();
}

// the account id was previously extracted and saved in a settings file
void HarvestHandler::load_account_id() {
    account_id = settings_manager->get_setting(account_id_key).toString();
    user_id = settings_manager->get_setting(user_id_key).toString();
    // Setting this for a while for retro-compatibility
    if (user_id.isEmpty()) {
        user_id = get_user_id();
        settings_manager->add_setting(user_id_key, user_id);
    }
}

bool HarvestHandler::is_ready() const {
    return auth_found;
}

// This method preloads a list of tasks from the harvest system based on the dates provided
// It is used to preload our TaskScrollArea with the tasks the user had previously added
void HarvestHandler::list_tasks(const QDate &from_date, const QDate &to_date) {
    if (!is_network_reachable) {
        QMessageBox::warning(nullptr, QApplication::translate("HarvestHandler", "Network Unreachable"),
                             QApplication::translate("HarvestHandler",
                                                     "You are currently not connected to the internet, please reconnect and try again"));
        return;
    }

    QUrl request_url(time_entries_url);
    QUrlQuery url_query;
    url_query.addQueryItem("from", from_date.toString(Qt::ISODate));
    url_query.addQueryItem("to", to_date.toString(Qt::ISODate));
    url_query.addQueryItem("user_id", user_id);
    request_url.setQuery(url_query);

    QNetworkReply *reply{do_request_with_auth(request_url, false, "GET")};
    connect(reply, &QNetworkReply::finished, this, &HarvestHandler::tasks_list_ready);
}

void HarvestHandler::add_task(Task *task) {
    if (!is_network_reachable) {
        QMessageBox::warning(nullptr, QApplication::translate("HarvestHandler", "Network Unreachable"),
                             QApplication::translate("HarvestHandler",
                                                     "You are currently not connected to the internet, please reconnect and try again"));
        return;
    }

    const QString spent_date{task->date.toString(Qt::ISODate)};
    const float seconds{static_cast<float>(QTime(0, 0).secsTo(task->time_tracked))};

    QJsonObject request_payload;
    request_payload.insert("project_id", task->project_id);
    request_payload.insert("task_id", task->task_id);
    request_payload.insert("spent_date", QJsonValue(spent_date));
    request_payload.insert("notes", task->note);
    request_payload.insert("hours", seconds / 60 / 60);
    request_payload.insert("is_running", seconds == 0);

    // We save our task in a map so that we can retrieve it later when the response comes
    size_t key{qHash(QString::number(task->project_id).append(QString::number(task->task_id)))};
    tasks_queue.insert({key, task});

    QNetworkReply *reply{do_request_with_auth(time_entries_url, false, "POST", QJsonDocument(request_payload))};
    connect(reply, &QNetworkReply::finished, this, &HarvestHandler::add_task_checks);
}

void HarvestHandler::update_task(const Task *updated_task) {
    if (!is_network_reachable) {
        QMessageBox::warning(nullptr, QApplication::translate("HarvestHandler", "Network Unreachable"),
                             QApplication::translate("HarvestHandler",
                                                     "You are currently not connected to the internet, please reconnect and try again"));
        return;
    }

    const float seconds{static_cast<float>(QTime(0, 0).secsTo(updated_task->time_tracked))};

    QJsonObject request_payload;
    request_payload.insert("project_id", updated_task->project_id);
    request_payload.insert("task_id", updated_task->task_id);
    request_payload.insert("notes", updated_task->note);
    request_payload.insert("hours", seconds / 60 / 60);

    // We save our task in a map so that we can retrieve it later when the response comes
    size_t key{qHash(QString::number(updated_task->project_id).append(QString::number(updated_task->task_id)))};
    tasks_queue.insert({key, const_cast<Task *>(updated_task)});

    QString update_url{time_entries_url + "/" + QString::number(updated_task->time_entry_id)};
    QNetworkReply *reply{do_request_with_auth(update_url, false, "PATCH", QJsonDocument(request_payload))};
    connect(reply, &QNetworkReply::finished, this, &HarvestHandler::update_task_checks);
}


void HarvestHandler::start_task(const Task &task) {
    if (!is_network_reachable) {
        QMessageBox::warning(nullptr, QApplication::translate("HarvestHandler", "Network Unreachable"),
                             QApplication::translate("HarvestHandler",
                                                     "You are currently not connected to the internet, please reconnect and try again"));
        return;
    }

    const QUrl url{time_entries_url + "/" + QString::number(task.time_entry_id) + "/restart"};
    QNetworkReply *reply{do_request_with_auth(url, false, "PATCH")};
    connect(reply, &QNetworkReply::finished, this, &HarvestHandler::start_task_checks);
}

void HarvestHandler::stop_task(const Task &task) {
    if (!is_network_reachable) {
        QMessageBox::warning(nullptr, QApplication::translate("HarvestHandler", "Network Unreachable"),
                             QApplication::translate("HarvestHandler",
                                                     "You are currently not connected to the internet, please reconnect and try again"));
        return;
    }

    const QUrl url{time_entries_url + "/" + QString::number(task.time_entry_id) + "/stop"};
    QNetworkReply *reply{do_request_with_auth(url, false, "PATCH")};
    connect(reply, &QNetworkReply::finished, this, &HarvestHandler::stop_task_checks);
}

void HarvestHandler::delete_task(const Task &task) {
    if (!is_network_reachable) {
        QMessageBox::warning(nullptr, QApplication::translate("HarvestHandler", "Network Unreachable"),
                             QApplication::translate("HarvestHandler",
                                                     "You are currently not connected to the internet, please reconnect and try again"));
        return;
    }

    const QUrl url{time_entries_url + "/" + QString::number(task.time_entry_id)};
    QNetworkReply *reply{do_request_with_auth(url, false, "DELETE")};
    connect(reply, &QNetworkReply::finished, this, &HarvestHandler::delete_task_checks);
}

QNetworkReply *HarvestHandler::do_request_with_auth(const QUrl &url, const bool sync_request, const QByteArray &verb,
                                                    const std::optional<QJsonDocument> &payload) {
    check_authenticate();

    QNetworkRequest request(url);
    request.setTransferTimeout(HarvestHandler::request_timeout_constant);
    request.setRawHeader("Authorization", "Bearer " + json_auth["access_token"].toString().toUtf8());
    request.setRawHeader("Harvest-Account-Id", account_id.toUtf8());

    QNetworkReply *reply;
    if (payload.has_value()) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        reply = network_manager.sendCustomRequest(request, verb, payload->toJson(QJsonDocument::JsonFormat::Compact));
    } else {
        reply = network_manager.sendCustomRequest(request, verb, "");
    }

    if (sync_request) {
        // Execute the event loop here, now we will wait here until readyRead() signal is emitted
        // which in turn will trigger event loop quit.
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
    }

    return reply;
}

void HarvestHandler::tasks_list_ready() {
    QNetworkReply *reply{dynamic_cast<QNetworkReply *>(sender())};
    if (default_error_check(reply, QApplication::translate("HarvestHandler", "Error Loading Tasks"),
                            QApplication::translate("HarvestHandler", "Could not load your tasks: "))) {
        return;
    }

    // Get response from the reply object
    const QJsonDocument list_tasks_response{read_close_reply(reply)};
    const QJsonValue tasks_object{list_tasks_response.object().value("time_entries")};

    for (const QJsonValueRef &&harvestTask: tasks_object.toArray()) {
        const QJsonObject task_object{harvestTask.toObject()};
        const long long int task_entry_id{task_object["id"].toInteger()};
        const long long int project_id{task_object["project"]["id"].toInteger()};
        const QString project_name{task_object["project"]["name"].toString()};
        const QString client_name{task_object["client"]["name"].toString()};
        const long long int task_id{task_object["task"]["id"].toInteger()};
        const QString task_name{task_object["task"]["name"].toString()};
        const QString note{task_object["notes"].toString()};

        const double time_tracked_decimal{task_object["hours"].toDouble()};
        double tracked_hours, tracked_minutes;
        tracked_minutes = std::modf(time_tracked_decimal, &tracked_hours);
        const QTime time_tracked(static_cast<int>(tracked_hours), static_cast<int>(tracked_minutes * 60));

        const bool started{!task_object["started_time"].toString().isNull()};
        const QDate task_date{QDate::fromString(task_object["spent_date"].toString(), Qt::DateFormat::ISODate)};

        Task *task = new Task{
                .project_id = project_id,
                .task_id = task_id,
                .time_entry_id = task_entry_id,
                .client_name = client_name,
                .project_name = project_name,
                .task_name = task_name,
                .time_tracked = time_tracked,
                .note = note,
                .started = started,
                .date = task_date,
        };
        emit task_added(task);
    }
}

//
void HarvestHandler::add_task_checks() {
    QNetworkReply *reply{dynamic_cast<QNetworkReply *>(sender())};
    if (default_error_check(reply, QApplication::translate("HarvestHandler", "Error Adding Task"),
                            QApplication::translate("HarvestHandler", "Could not add your task: "))) {
        return;
    }

    // Get response from the reply object
    const QJsonDocument add_task_response{read_close_reply(reply)};

    const long long int project_id{add_task_response["project"]["id"].toInteger()};
    const long long int task_id{add_task_response["task"]["id"].toInteger()};
    const size_t key{qHash(QString::number(project_id).append(QString::number(task_id)))};

    const auto task_element = tasks_queue.find(key);
    if (task_element == tasks_queue.end()) {
        return;
    }

    // if we could find a task, let's fetch it and remove it from the queue map
    Task *task = task_element->second;
    tasks_queue.erase(task_element);
    task->time_entry_id = add_task_response["id"].toInteger();

    emit task_added(task);
}

void HarvestHandler::update_task_checks() {
    auto *reply{dynamic_cast<QNetworkReply *>(sender())};
    default_error_check(reply, QApplication::translate("HarvestHandler", "Error Updating Task"),
                        QApplication::translate("HarvestHandler", "Could not update this task: "));
    reply->deleteLater();
}

void HarvestHandler::start_task_checks() {
    auto *reply{dynamic_cast<QNetworkReply *>(sender())};
    default_error_check(reply, QApplication::translate("HarvestHandler", "Error Starting Task"),
                        QApplication::translate("HarvestHandler", "Could not start this task: "));
    reply->deleteLater();
}

void HarvestHandler::stop_task_checks() {
    auto *reply{dynamic_cast<QNetworkReply *>(sender())};
    default_error_check(reply, QApplication::translate("HarvestHandler", "Error Stopping Task"),
                        QApplication::translate("HarvestHandler", "Could not stop this task: "));
    reply->deleteLater();
}

void HarvestHandler::delete_task_checks() {
    auto *reply{dynamic_cast<QNetworkReply *>(sender())};
    default_error_check(reply, QApplication::translate("HarvestHandler", "Error Deleting Task"),
                        QApplication::translate("HarvestHandler", "Could not delete this task: "));
    reply->deleteLater();
}

bool HarvestHandler::default_error_check(QNetworkReply *reply, const QString &base_error_title,
                                         const QString &base_error_body) {
    if (reply->error() == QNetworkReply::NetworkError::TimeoutError) {
        const QString error_string{
                base_error_body + QApplication::translate("HarvestHandler", "the request timed out.")};
        QMessageBox::information(nullptr, base_error_title, error_string);

    }
    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        const QJsonDocument error_report{read_close_reply(const_cast<QNetworkReply *>(reply))};
        const QString error_string{base_error_body + error_report["error"].toString()};
        QMessageBox::information(nullptr, base_error_title, error_string);
        return true;
    }
    return false;
}

void HarvestHandler::set_network_reachability(const QNetworkInformation::Reachability &reachability) {
    is_network_reachable = reachability == QNetworkInformation::Reachability::Online;
}

QString HarvestHandler::get_http_message(const QString &message) {
    QString final_message;
    final_message.append("HTTP/1.0 200 OK\r\n");
    final_message.append("Content-Type: text/html; charset=utf-8\r\n\r\n");
    final_message.append("<html>");
    final_message.append("<body>");
    final_message.append(message);
    final_message.append("</body>");
    final_message.append("</html>");
    return final_message;
}

void HarvestHandler::logout_cleanup() {
    auth_file.remove();
    settings_manager->remove_setting(account_id_key);
}

QString HarvestHandler::get_user_id() {
    QUrl request_url(user_url);

    QNetworkReply *reply{do_request_with_auth(request_url, true, "GET")};
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << reply->error();
    }

    const QJsonDocument json_payload{read_close_reply(reply)};
    return QString::number(json_payload["id"].toDouble(), 'f', 0);
}
