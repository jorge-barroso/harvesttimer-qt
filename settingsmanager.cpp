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

void SettingsManager::add_setting(const QString& key, const QString& value)
{
	settings.setValue(key, value);
}

QVariant SettingsManager::get_setting(const QString& key)
{
	return settings.value(key);

}