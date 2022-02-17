#ifndef HARVESTHANDLER_H
#define HARVESTHANDLER_H

#include <QObject>
#include <QString>
#include <QDir>
#include <QFile>
#include <QTcpServer>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QEventLoop>
#include "settingsmanager.h"
#include "project.h"

class HarvestHandler : public QObject
{
	Q_OBJECT


	public slots:

		void newConnection();

	public:
		explicit HarvestHandler(const QDir& config_dir);

        ~HarvestHandler() override;

		std::vector<Project> get_user_data();

		[[nodiscard]] bool is_ready() const;

	signals:

		void ready();

		void data_ready(std::vector<Project>);

	private slots:

		void code_received();

		void authentication_received();

	private:
		QTcpServer* auth_server;
		QTcpSocket* auth_socket;

		const QString client_id{ "VkKA3WoB2M5cEQGwf82VkeHb" };
		const QString client_secret{
				"QUwB8dtQxMwY5omBHgZBsXAhB2h_jzKZcGZkCUom1CPBYvTKUGPty7ree7ao92PV5FT5VQHbVWwNzTQUITVLmg" };
		const QString grant_type{ "authorization_code" };
		const QString auth_host{ "https://id.getharvest.com" };
		const QString login_url{
				auth_host + "/oauth2/authorize?client_id=" + client_id + "&response_type=code" };
		const QString auth_url{ auth_host + "/api/v2/oauth2/token" };

		const QString requests_host{ "https://api.harvestapp.com" };
		const QString assignments_url{ requests_host + "/v2/users/me/project_assignments" };

		const QString auth_file_name = "harvest_auth.json";
		QFile auth_file;
        SettingsManager* settings_manager;

		QNetworkAccessManager network_manager;
		QNetworkReply* reply;

		QJsonDocument json_auth;
		QString account_id;

		bool auth_found;

		QEventLoop loop;

		const QString pagination_records{ "100" };

		QJsonDocument get_authentication();

		QJsonDocument get_auth_details();

//		QJsonDocument refresh_token();

		void login();

		bool json_auth_is_complete();

		[[maybe_unused]] bool json_auth_is_safely_active();

		static std::map<QString, QString> parse_query_string(QString& query_string);

//		void authenticate(QString& auth_code);

		void save_authentication();

		void authenticate_request(QString* auth_code = nullptr, QString* refresh_token = nullptr);

		void get_new_account_id(QString& scope);

		void load_account_id();

		void request_with_auth(const QUrl& url, const bool sync_request);

		static void get_projects_data(const QJsonDocument& json_payload, std::vector<Project>& projects_vector);
};

#endif // HARVESTHANDLER_H
