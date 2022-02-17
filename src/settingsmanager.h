//
// Created by jorge on 16/02/2022.
//

#ifndef HARVESTTIMER_QT_CMAKE_SETTINGSMANAGER_H
#define HARVESTTIMER_QT_CMAKE_SETTINGSMANAGER_H


#include <QDir>
#include <QSettings>

class SettingsManager
{
	public:
		static SettingsManager* get_instance(const QDir& config_dir);

        ~SettingsManager();

		void add_setting(const QString& key, const QString& value);

		QVariant get_setting(const QString& key);
	private:
		const QString settings_filename{ "settings.ini" };
		const QString settings_file_path;
		QSettings settings;

		static SettingsManager* instance;

        explicit SettingsManager(const QDir& config_dir);
};


#endif //HARVESTTIMER_QT_CMAKE_SETTINGSMANAGER_H
