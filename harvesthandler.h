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
#include <QNetworkInformation>
#include "settingsmanager.h"
#include "harvestproject.h"
#include "task.h"

class HarvestHandler : public QObject
{
	Q_OBJECT

	public slots:

		void new_connection();

	public:
		static HarvestHandler* get_instance(const QDir& config_dir);

		static void reset_instance();

		std::vector<HarvestProject> update_user_data();

		[[nodiscard]] bool is_ready() const;

		void add_task(Task* task);

		void update_task(const Task* updated_task);

		void start_task(const Task& task);

		void stop_task(const Task& task);

		void delete_task(const Task& task);

		void list_tasks(const QDate& from_date, const QDate& to_date);

		void set_network_reachability(const QNetworkInformation::Reachability& reachability);

	signals:

		void ready();

		void data_ready(std::vector<HarvestProject>);

		void task_added(Task*);

	protected:
		// We're taking a singleton approach here so the constructor will remain protected
		explicit HarvestHandler(const QDir& config_dir); // Prevent construction
		~HarvestHandler() override; // Prevent unwanted destruction

	private slots:

		void code_received();

		void authentication_received(const QNetworkReply* reply);

		void tasks_list_ready();

		void update_task_checks();

		void add_task_checks();

		void start_task_checks();

		void stop_task_checks();

		void delete_task_checks();

	private:
		static HarvestHandler* harvest_handler;

		QTcpServer* auth_server;
		QTcpSocket* auth_socket;

		const QString client_id{ "VkKA3WoB2M5cEQGwf82VkeHb" };
		const QString client_secret{
				"QUwB8dtQxMwY5omBHgZBsXAhB2h_jzKZcGZkCUom1CPBYvTKUGPty7ree7ao92PV5FT5VQHbVWwNzTQUITVLmg" };
		static const QString default_grant_type;
		static const QString refresh_grant_type;

		// Authentication endpoints
		const QString auth_host{ "https://id.getharvest.com" };
		const QString login_url{ auth_host + "/oauth2/authorize?client_id=" + client_id + "&response_type=code" };
		const QString auth_url{ auth_host + "/api/v2/oauth2/token" };

		// Task-related endpoints
		const QString requests_host{ "https://api.harvestapp.com" };
		const QString assignments_url{ requests_host + "/v2/users/me/project_assignments" };
		const QString time_entries_url{ requests_host + "/v2/time_entries" };

		std::vector<HarvestProject> projects;

		const QString auth_file_name = "harvest_auth.json";
		QFile auth_file;
		SettingsManager* settings_manager;

		QNetworkAccessManager network_manager;

		QJsonDocument json_auth;
		QString account_id;

		bool auth_found;

		QEventLoop loop;

		const QString pagination_records{ "100" };

		QJsonDocument get_authentication();

		QJsonDocument get_auth_details();

		void login();

		bool json_auth_is_complete();

		[[maybe_unused]] bool json_auth_is_safely_active();

		static std::map<QString, QString> parse_query_string(QString& query_string);

		void save_authentication();

		void authenticate_request(QString* auth_code, QString* refresh_token);

		void get_new_account_id(QString& scope);

		void load_account_id();

		QNetworkReply* do_request_with_auth(const QUrl& url, bool sync_request, const QByteArray& verb,
											const std::optional<QJsonDocument>& payload = std::nullopt);

		static void get_projects_data(const QJsonDocument& json_payload, std::vector<HarvestProject>& projects_vector);

		static QJsonDocument read_close_reply(QNetworkReply* reply);

		static bool default_error_check(QNetworkReply* reply, const QString& base_error_title,
										const QString& base_error_body);

		void check_authenticate();

        static QString get_http_message(const QString& message);

		std::unordered_map<size_t, Task*> tasks_queue;

		static const int request_timeout_constant;

		bool is_network_reachable;
};

#endif // HARVESTHANDLER_H
