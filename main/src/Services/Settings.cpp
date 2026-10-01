#include "Settings.h"
#include <nvs_flash.h>

DEFINE_LOG(Settings)

Settings::Settings(){
	ESP_ERROR_CHECK(nvs_open(NVSNamespace, NVS_READWRITE, &handle));
	load();
}

Settings::~Settings(){
	nvs_close(handle);
}

SettingsStruct Settings::get() const{
	return settingsStruct;
}

void Settings::set(const SettingsStruct& settings){
	settingsStruct = settings;
}

void Settings::store() const{
	esp_err_t err = nvs_set_blob(handle, BlobName, &settingsStruct, sizeof(SettingsStruct));
	if(err != ESP_OK){
		CMF_LOG(Settings, LogLevel::Error, "Error storing settings: %s", esp_err_to_name(err));
		return;
	}
	nvs_commit(handle);
}

RobotConfigData Settings::getRobotConfig() const{
	return robotConfig;
}

void Settings::setRobotConfig(const RobotConfigData& config){
	robotConfig = config;
	nvs_set_u8(handle, VolumeKey, config.volume);
	nvs_set_u8(handle, NightModeKey, config.nightMode);
	nvs_set_u8(handle, NightVolumeKey, config.nightVolume);
	nvs_set_u8(handle, RoamingKey, config.roaming);
	nvs_commit(handle);
}

void Settings::load(){
	size_t len = sizeof(SettingsStruct);
	esp_err_t err = nvs_get_blob(handle, BlobName, &settingsStruct, &len);
	if(err != ESP_OK){
		CMF_LOG(Settings, LogLevel::Warning, "No stored settings found, using defaults: %s", esp_err_to_name(err));
		settingsStruct = SettingsStruct();
	}

	uint8_t val = 0;
	if(nvs_get_u8(handle, VolumeKey, &val) == ESP_OK && val >= 10 && val <= 100) robotConfig.volume = val;
	if(nvs_get_u8(handle, NightModeKey, &val) == ESP_OK && val <= static_cast<uint8_t>(NightMode::From00)) robotConfig.nightMode = val;
	if(nvs_get_u8(handle, NightVolumeKey, &val) == ESP_OK && val >= 10 && val <= 100) robotConfig.nightVolume = val;
	if(nvs_get_u8(handle, RoamingKey, &val) == ESP_OK && val <= 1) robotConfig.roaming = val;
}