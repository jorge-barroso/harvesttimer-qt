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

HarvestHandler* HarvestHandler::harvest_handler{ nullptr };
const QString HarvestHandler::default_grant_type{ "authorization_code" };
const QString HarvestHandler::refresh_grant_type{ "refresh_token" };

HarvestHandler* HarvestHandler::get_instance(const QDir& config_dir)
{
	if (harvest_handler == nullptr)
	{
		harvest_handler = new HarvestHandler(config_dir);
	}

	return harvest_handler;
}

void HarvestHandler::reset_instance()
{
	delete harvest_handler;
	harvest_handler = nullptr;
}

HarvestHandler::HarvestHandler(const QDir& config_dir)
		: auth_server{ nullptr }, auth_socket{ nullptr }, auth_file(config_dir.absolutePath() + "/" + auth_file_name),
		  settings_manager{ SettingsManager::get_instance(config_dir) }, network_manager(this), loop()
{
	network_manager.setAutoDeleteReplies(true);
	// try and load the authentication details stored in the user configuration directories
	this->json_auth = get_authentication();
	// this auth is considered found not only if we read it, but we also need to make sure all the necessary fields are present
	auth_found = !json_auth.isEmpty() && json_auth_is_complete();

	if (!auth_found)
	{
		login();
	}
	else
	{
		load_account_id();
		emit ready();
	}
}

HarvestHandler::~HarvestHandler()
{
	SettingsManager::reset_instance();

	delete auth_server;
	delete auth_socket;

	// https://stackoverflow.com/questions/19157946/does-stdmap-destructor-call-key-destructors-as-well-as-value-destructors
}

// this is an additional check that validates whether there's at least one more day of validity in the auth
// token or we need to refresh it before any requests can be performed
void HarvestHandler::check_authenticate()
{
	if (!json_auth_is_safely_active())
	{
		QString refresh_token(json_auth["refresh_token"].toString());
		authenticate_request(nullptr, &refresh_token);
	}
}

QJsonDocument HarvestHandler::get_authentication()
{
	if (!auth_file.exists())
	{
		return { };
	}

	return get_auth_details();
}

void HarvestHandler::login()
{
	QDesktopServices::openUrl(login_url);

	auth_server = new QTcpServer();

	if (!auth_server->listen(QHostAddress::Any, 23456))
	{
		const QString error_string{ "Error while waiting for Harvest authorization" };
		QMessageBox::information(nullptr, "Error", error_string);
		QApplication::quit();
	}

	connect(auth_server, &QTcpServer::newConnection, this, &HarvestHandler::new_connection);
}

QJsonDocument HarvestHandler::get_auth_details()
{
	auth_file.open(QFile::ReadOnly | QFile::Text);
	QTextStream text_stream(&auth_file);

	const QByteArray auth_text{ text_stream.readAll().toUtf8() };
	QJsonDocument auth_details{ QJsonDocument::fromJson(auth_text) };

	auth_file.close();

	return auth_details;
}

bool HarvestHandler::json_auth_is_complete()
{
	const QJsonObject json_object{ json_auth.object() };

	bool contains_access_token{ json_object.contains("access_token") };
	bool contains_refresh_token{ json_object.contains("refresh_token") };
	bool contains_token_type{ json_object.contains("token_type") };
	bool contains_expires_in{ json_object.contains("expires_in") };

	return contains_access_token && contains_refresh_token && contains_token_type && contains_expires_in;
}

bool HarvestHandler::json_auth_is_safely_active()
{
	auto expires_on{ QDateTime::fromMSecsSinceEpoch(json_auth["expires_on"].toInteger()) };

	// if there's less than a full day of time left before the token expires...
	return expires_on > QDateTime::currentDateTime().addDays(1);
}

void HarvestHandler::new_connection()
{
	auth_socket = auth_server->nextPendingConnection();
	auth_server->close();
	connect(auth_socket, &QTcpSocket::readyRead, this, &HarvestHandler::code_received);
}

void HarvestHandler::code_received()
{
	if (auth_socket->canReadLine())
	{
		QStringList tokens = QString(auth_socket->readLine()).split(QRegularExpression("[ \r\n][ \r\n]*"));

		auth_socket->write("Authentication successful, you may now close this tab");
		auth_socket->flush();
		auth_socket->close();
		delete auth_socket;
		delete auth_server;

		for (QString& token: tokens)
		{
			if (token.contains("?"))
			{
				std::map<QString, QString> query_map{ parse_query_string(token) };
				if (!query_map.contains("code") || !query_map.contains("scope"))
				{
					const char* error_string{
							"The details received from Harvest did not contain the minimum details required" };
					QMessageBox::information(nullptr, "Incomplete Details", error_string);
					QApplication::quit();
				}
				get_new_account_id(query_map["scope"]);
				authenticate_request(&query_map["code"], nullptr);
			}
		}
	}
}

std::map<QString, QString> HarvestHandler::parse_query_string(QString& query_string)
{
	// remove the starting "/?" from the query string
	query_string.remove(0, 2);


	QStringList values{ query_string.split("&") };
	std::map<QString, QString> query_map;
	for (QString& value: values)
	{
		QStringList detailed_value{ value.split("=") };
		query_map.emplace(detailed_value[0], detailed_value[1]);
	}

	return query_map;
}

void HarvestHandler::authentication_received(const QNetworkReply* reply)
{
	if (reply->error() != QNetworkReply::NetworkError::NoError)
	{
		const QString error_string{ "Error while authenticating with Harvest services: " + reply->errorString() };
		QMessageBox::information(nullptr, "Error authenticating", error_string);
		QApplication::quit();
		return;
	}

	// reply is originally non-const so this cast should be safe while allowing to use it
	// as const in the previous error handling
	json_auth = read_reply(const_cast<QNetworkReply*>(reply));
	QJsonObject json_object{ json_auth.object() };
	qint64 seconds{ json_object["expires_in"].toInteger() };
	json_object.insert("expires_on", QDateTime::currentDateTime().addSecs(seconds).toMSecsSinceEpoch());
	json_auth.setObject(json_object);

	save_authentication();

	emit ready();
}

QJsonDocument HarvestHandler::read_reply(QNetworkReply* reply)
{
	QByteArray response_body{ static_cast<QString>(reply->readAll()).toUtf8() };
	// qDebug() << "Harvest response body:" << response_body;
	reply->close();
	return QJsonDocument::fromJson(response_body);
}

void HarvestHandler::save_authentication()
{
	if (!auth_file.open(QFile::WriteOnly | QFile::Text))
	{
		const char* error_text = "It was not possible to save your credentials, you will have to log in again the next time you open Harvest Timer";
		QMessageBox::information(nullptr, "Error saving authentication", error_text);
		return;
	}

	auth_file.write(json_auth.toJson(QJsonDocument::Compact));

	auth_file.close();
}

void HarvestHandler::authenticate_request(QString* auth_code, QString* refresh_token)
{
	QUrl request_url(auth_url);
	QNetworkRequest request(request_url);
	request.setHeader(QNetworkRequest::UserAgentHeader, "Harvest Timer Qt (jorge_barroso_11@hotmail.com)");
	request.setRawHeader("Accept", "application/json");

	QUrlQuery url_query;
	url_query.addQueryItem("client_id", client_id);
	url_query.addQueryItem("client_secret", client_secret);

	QString grant_type;
	if (auth_code != nullptr)
	{
		url_query.addQueryItem("code", *auth_code);
		grant_type = HarvestHandler::default_grant_type;
	}
	else if (refresh_token != nullptr)
	{
		url_query.addQueryItem("refresh_token", *refresh_token);
		grant_type = HarvestHandler::refresh_grant_type;
	}
	url_query.addQueryItem("grant_type", grant_type);


	QNetworkReply* reply{ network_manager.post(request, url_query.toString().toUtf8()) };

	connect(reply, &QNetworkReply::readyRead, &loop, &QEventLoop::quit);
	loop.exec();

	authentication_received(reply);
	reply->deleteLater();
}


std::vector<HarvestProject> HarvestHandler::update_user_data()
{
	QUrl request_url(assignments_url);

	int total_pages = -1;
	for (int page = 1; total_pages == -1 || page <= total_pages; ++page)
	{
		QUrlQuery url_query;
		url_query.addQueryItem("page", QString::number(page));
		url_query.addQueryItem("per_page", pagination_records);
		request_url.setQuery(url_query);

		QNetworkReply* reply{ do_request_with_auth(request_url, true, "GET") };
		if (reply->error() != QNetworkReply::NoError)
		{
			qDebug() << reply->error();
		}

		const QJsonDocument json_payload{ read_reply(reply) };
		get_projects_data(json_payload, projects);

		if (total_pages == -1)
		{
			total_pages = json_payload["total_pages"].toInt();
		}
	}

	return projects;
}

void HarvestHandler::get_projects_data(const QJsonDocument& json_payload, std::vector<HarvestProject>& projects_vector)
{
	for (const QJsonValue project_assignment: json_payload["project_assignments"].toArray())
	{
		std::vector<HarvestTask> project_tasks_vector;
		for (const QJsonValue json_task: project_assignment["task_assignments"].toArray())
		{
			const QJsonValue json_task_details{ json_task["task"] };
			const HarvestTask task = HarvestTask{
//					json_task["id"].toInteger(),
					json_task_details["id"].toInteger(),
					json_task_details["name"].toString()
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

void HarvestHandler::get_new_account_id(QString& scope)
{
	account_id = scope.split("%3A")[1];
	settings_manager->add_setting("account_id", account_id);
}

// the account id was previously extracted and saved in a settings file
void HarvestHandler::load_account_id()
{
	account_id = settings_manager->get_setting("account_id").toString();
}

bool HarvestHandler::is_ready() const
{
	return auth_found;
}

void HarvestHandler::list_tasks(const QDate& from_date, const QDate& to_date)
{
	QUrl request_url(time_entries_url);
	QUrlQuery url_query;
	url_query.addQueryItem("from", from_date.toString(Qt::ISODate));
	url_query.addQueryItem("to", to_date.toString(Qt::ISODate));
	request_url.setQuery(url_query);

	QNetworkReply* reply{ do_request_with_auth(request_url, false, "GET") };
	connect(reply, &QNetworkReply::readyRead, this, &HarvestHandler::tasks_list_ready);
}

void HarvestHandler::add_task(Task* task)
{
	const QString spent_date{ task->date.toString(Qt::ISODate) };
	const float seconds{ static_cast<float>(QTime(0, 0).secsTo(task->time_tracked)) };

	QJsonObject request_payload;
	request_payload.insert("project_id", task->project_id);
	request_payload.insert("task_id", task->task_id);
	request_payload.insert("spent_date", QJsonValue(spent_date));
	request_payload.insert("notes", task->note);
	request_payload.insert("hours", seconds / 60 / 60);
	request_payload.insert("is_running", seconds == 0);

	// We save our task in a map so that we can retrieve it later when the response comes
	size_t key{ qHash(QString::number(task->project_id).append(QString::number(task->task_id))) };
	tasks_queue.insert({ key, task });

	QNetworkReply* reply{ do_request_with_auth(time_entries_url, false, "POST", QJsonDocument(request_payload)) };
	connect(reply, &QNetworkReply::readyRead, this, &HarvestHandler::add_task_checks);
}

void HarvestHandler::start_task(const Task& task)
{
	const QUrl url{ time_entries_url + "/" + QString::number(task.time_entry_id) + "/restart" };
	QNetworkReply* reply{ do_request_with_auth(url, false, "PATCH") };
	connect(reply, &QNetworkReply::readyRead, this, &HarvestHandler::start_task_checks);
}

void HarvestHandler::stop_task(const Task& task)
{
	const QUrl url{ time_entries_url + "/" + QString::number(task.time_entry_id) + "/stop" };
	QNetworkReply* reply{ do_request_with_auth(url, false, "PATCH") };
	connect(reply, &QNetworkReply::readyRead, this, &HarvestHandler::stop_task_checks);
}

void HarvestHandler::delete_task(const Task& task)
{
	const QUrl url{ time_entries_url + "/" + QString::number(task.time_entry_id) };
	QNetworkReply* reply{ do_request_with_auth(url, false, "DELETE") };
	connect(reply, &QNetworkReply::readyRead, this, &HarvestHandler::delete_task_checks);
}

QNetworkReply* HarvestHandler::do_request_with_auth(const QUrl& url, const bool sync_request, const QByteArray& verb,
													const std::optional<QJsonDocument>& payload)
{
	// qDebug() << "New request to: " << url;

	check_authenticate();

	QNetworkRequest request(url);
	request.setRawHeader("Authorization", "Bearer " + json_auth["access_token"].toString().toUtf8());
	request.setRawHeader("Harvest-Account-Id", account_id.toUtf8());

	QNetworkReply* reply;
	if (payload.has_value())
	{
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
		reply = network_manager.sendCustomRequest(request, verb, payload->toJson(QJsonDocument::JsonFormat::Compact));
	}
	else
	{
		reply = network_manager.sendCustomRequest(request, verb, "");
	}

	if (sync_request)
	{
		// Execute the event loop here, now we will wait here until readyRead() signal is emitted
		// which in turn will trigger event loop quit.
		connect(reply, &QNetworkReply::readyRead, &loop, &QEventLoop::quit);
		loop.exec();
	}

	return reply;
}

void HarvestHandler::tasks_list_ready()
{
	QNetworkReply* reply{ dynamic_cast<QNetworkReply*>(sender()) };
	if (default_error_check(reply, "Could not add your task: ", "Error Adding Task"))
	{
		return;
	}

	// Get response from the reply object
	const QJsonDocument list_tasks_response{ read_reply(reply) };
	const QJsonValue tasks_object{ list_tasks_response.object().value("time_entries") };

	for (const QJsonValueRef&& harvestTask: tasks_object.toArray())
	{
		const QJsonObject task_object{ harvestTask.toObject() };
		// qDebug() << "Task: " << harvestTask;

		const long long int task_entry_id{ task_object["id"].toInteger() };
		const long long int project_id{ task_object["project"]["id"].toInteger() };
		const QString project_name{ task_object["project"]["name"].toString() };
		const long long int task_id{ task_object["task"]["id"].toInteger() };
		const QString task_name{ task_object["task"]["name"].toString() };
		const QString note{ task_object["notes"].toString() };

		const double time_tracked_decimal{ task_object["hours"].toDouble() };
		double tracked_hours, tracked_minutes;
		tracked_minutes = std::modf(time_tracked_decimal, &tracked_hours);
		const QTime time_tracked(static_cast<int>(tracked_hours), static_cast<int>(tracked_minutes * 60));

		const bool started{ !task_object["started_time"].toString().isNull() };
		const QDate task_date{ QDate::fromString(task_object["spent_date"].toString(), Qt::DateFormat::ISODate) };

		const Task* task = new Task{
				project_id,
				task_id,
				task_entry_id,
				project_name,
				task_name,
				time_tracked,
				note,
				started,
				task_date
		};
		emit task_added(task);
	}
}

//
void HarvestHandler::add_task_checks()
{
	QNetworkReply* reply{ dynamic_cast<QNetworkReply*>(sender()) };
	if (default_error_check(reply, "Could not add your task: ", "Error Adding Task"))
	{
		return;
	}

	// Get response from the reply object
	const QJsonDocument add_task_response{ read_reply(reply) };

	const long long int project_id{ add_task_response["project"]["id"].toInteger() };
	const long long int task_id{ add_task_response["task"]["id"].toInteger() };
	const size_t key{ qHash(QString::number(project_id).append(QString::number(task_id))) };

	const auto task_element = tasks_queue.find(key);
	if (task_element == tasks_queue.end())
	{
		return;
	}

	// if we could find a task, let's fetch it and remove it from the queue map
	Task* task = task_element->second;
	tasks_queue.erase(task_element);
	task->time_entry_id = add_task_response["id"].toInteger();

	emit task_added(task);
}

void HarvestHandler::start_task_checks()
{
	auto* reply{ dynamic_cast<QNetworkReply*>(sender()) };
	default_error_check(reply, "Could not start this task: ", "Error Starting Task");
	reply->deleteLater();
}

void HarvestHandler::stop_task_checks()
{
	auto* reply{ dynamic_cast<QNetworkReply*>(sender()) };
	default_error_check(reply, "Could not set_stopped this task: ", "Error Stopping Task");
	reply->deleteLater();
}

void HarvestHandler::delete_task_checks()
{
	auto* reply{ dynamic_cast<QNetworkReply*>(sender()) };
	default_error_check(reply, "Could not delete this task: ", "Error Deleting Task");
	reply->deleteLater();
}

bool HarvestHandler::default_error_check(QNetworkReply* reply, const QString& base_error_title,
										 const QString& base_error_body)
{
	if (reply->error() != QNetworkReply::NetworkError::NoError)
	{
		const QJsonDocument error_report{ read_reply(const_cast<QNetworkReply*>(reply)) };
		const QString error_string{ base_error_body + error_report["error"].toString() };
		QMessageBox::information(nullptr, base_error_title, error_string);
		return true;
	}
	return false;
}