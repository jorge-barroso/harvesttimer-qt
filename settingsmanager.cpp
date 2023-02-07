//
// Created by jorge on 16/02/2022.
//

#include "settingsmanager.h"

SettingsManager* SettingsManager::instance = nullptr;

SettingsManager* SettingsManager::get_instance(const QDir& config_dir)
{
	if (!instance)
	{
		instance = new SettingsManager(config_dir);
	}

	return instance;
}

void SettingsManager::reset_instance()
{
	delete instance;
	instance = nullptr;
}

SettingsManager::SettingsManager(const QDir& config_dir)
		: settings_file_path(config_dir.absolutePath() + "/" + settings_filename),
		  settings(settings_file_path, QSettings::NativeFormat)
{}

SettingsManager::~SettingsManager() = default;

void SettingsManager::add_setting(const QString& group, const QString& key, const QVariant &value)
{
    settings.beginGroup(group);
	settings.setValue(key, value);
    settings.endGroup();
}

QVariant SettingsManager::get_setting(const QString &group, const QString &key)
{
	QVariant value{settings.value(group + "/" + key)};
    // Setting this for a while for retro-compatibility
    if (value.isNull()) {
        value = settings.value(key);
    }
    return value;
}

void SettingsManager::remove_setting(const QString& key) {
    settings.remove(key);
}
