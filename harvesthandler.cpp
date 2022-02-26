#include "harvesthandler.h"
#include "harvesttask.h"
#include "harvestproject.h"
#include "task.h"
#include <QTextStream>
#include <chrono>
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

HarvestHandler::HarvestHandler(const QDir& config_dir)
		: auth_server{ nullptr }, auth_socket{ nullptr }, auth_file(config_dir.absolutePath() + "/" + auth_file_name),
		  settings_manager{ SettingsManager::get_instance(config_dir) }, network_manager(this), reply{ nullptr }, loop()
{
	this->json_auth = get_authentication();
	auth_found = !json_auth.isEmpty() && json_auth_is_complete();

	if (!auth_found)
	{
		login();
	}
//	else if (!json_auth_is_safely_active())
//	{
//		QString refresh_token(json_auth["refresh_token"].toString());
//		authenticate_request(nullptr, &refresh_token);
//	}
	else
	{
		load_account_id();
		emit ready();
	}
}

HarvestHandler::~HarvestHandler()
{
	delete HarvestHandler::harvest_handler;

	delete auth_server;
	delete auth_socket;

	delete settings_manager;

	delete reply;
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

	connect(auth_server, &QTcpServer::newConnection, this, &HarvestHandler::newConnection);
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

[[maybe_unused]] bool HarvestHandler::json_auth_is_safely_active()
{
	const long expiry_time = json_auth["expires_in"].toInteger();

	const std::chrono::time_point renewal_chrono = std::chrono::system_clock::now() + std::chrono::hours{ 24 };
	const auto renewal_epoch = std::chrono::duration_cast<std::chrono::seconds>(renewal_chrono.time_since_epoch());

	return expiry_time >= renewal_epoch.count();
}

void HarvestHandler::newConnection()
{
	auth_socket = auth_server->nextPendingConnection();
	auth_server->close();
	delete auth_server;
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
				authenticate_request(&query_map["code"]);
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

void HarvestHandler::authentication_received()
{
	if (reply->error() != QNetworkReply::NetworkError::NoError)
	{
		const QString error_string{ "Error while authenticating with Harvest services: " + reply->errorString() };
		QMessageBox::information(nullptr, "Error authenticating", error_string);
		QApplication::quit();
	}

	json_auth = readReply();

	save_authentication();

	emit ready();
}

QJsonDocument HarvestHandler::readReply()
{
	QByteArray response_body{ static_cast<QString>(reply->readAll()).toUtf8() };
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
	url_query.addQueryItem("grant_type", grant_type);

	if (auth_code != nullptr)
		url_query.addQueryItem("code", *auth_code);
	else if (refresh_token != nullptr)
		url_query.addQueryItem("refresh_token", *refresh_token);

	reply = network_manager.post(request, url_query.toString().toUtf8());

	connect(reply, &QNetworkReply::readyRead, &loop, &QEventLoop::quit);
	loop.exec();

	authentication_received();
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

		doRequestWithAuth(request_url, true, "GET");

		const QJsonDocument json_payload{ readReply() };
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
					json_task["id"].toInteger(),
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

void HarvestHandler::load_account_id()
{
	account_id = settings_manager->get_setting("account_id").toString();
}

bool HarvestHandler::is_ready() const
{
	return auth_found;
}

std::optional<long long int> HarvestHandler::addTask(const Task* task)
{
	const QString spent_date{ QDate::currentDate().toString(Qt::ISODate) };
	const float seconds{ static_cast<float>(QTime(0, 0).secsTo(task->time_tracked)) };

	QJsonObject request_payload;
	request_payload.insert("project_id", task->project_id);
	request_payload.insert("task_id", task->task_id);
	request_payload.insert("spent_date", QJsonValue(spent_date));
	request_payload.insert("notes", task->note);
	request_payload.insert("hours", seconds / 60 / 60);
	request_payload.insert("is_running", seconds == 0);

	doRequestWithAuth(time_entries_url, true, "POST", QJsonDocument(request_payload));

	if (reply->error() != QNetworkReply::NetworkError::NoError)
	{
		const QString error_string{ "Could not add your task: " + reply->errorString() };
		QMessageBox::information(nullptr, "Error Adding Task", error_string);
		return std::nullopt;
	}

	QJsonDocument add_task_response = readReply();

	return add_task_response["id"].toInteger();
}

void HarvestHandler::startTask(const Task& task)
{
	const QUrl url{ time_entries_url + "/" + QString::number(task.task_id) + "/restart" };
	doRequestWithAuth(url, true, "PATCH");
}

void HarvestHandler::stopTask(const Task& task)
{
	const QUrl url{ time_entries_url + "/" + QString::number(task.time_entry_id) + "/stop" };
	qDebug() << url.toString();
	doRequestWithAuth(url, true, "PATCH");

	if (reply->error() != QNetworkReply::NetworkError::NoError)
	{
		const QJsonDocument error_report{ readReply() };
		const QString error_string{ "Could not stop this task: " + error_report["error"].toString() };
		QMessageBox::information(nullptr, "Error Stopping Task", error_string);
	}
}

void HarvestHandler::doRequestWithAuth(const QUrl& url, const bool sync_request, const QByteArray& verb,
									   const std::optional<QJsonDocument>& payload)
{
	QNetworkRequest request(url);
	request.setRawHeader("Authorization", "Bearer " + json_auth["access_token"].toString().toUtf8());
	request.setRawHeader("Harvest-Account-Id", account_id.toUtf8());

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
}

HarvestHandler* HarvestHandler::getInstance(const QDir& config_dir)
{
	if (harvest_handler == nullptr)
		harvest_handler = new HarvestHandler(config_dir);

	return harvest_handler;
}

