#ifndef BUTTERBOT_FIRMWARE_SETTINGS_H
#define BUTTERBOT_FIRMWARE_SETTINGS_H

#include <nvs.h>
#include <BBData.h>
#include <CtrlData.h>

struct SettingsStruct {
	using Scale = TempHumScale;

	bool testVariable = true;
	Scale tempHumScale = Scale::Celsius;
	float ledStrobePeriod = 0.4f;
	float ledBreathePeriod = 2.0f;
	bool gasModuleConfigured = false;
};

class Settings {
public:
	Settings();
	virtual ~Settings();

	SettingsStruct get() const;
	void set(const SettingsStruct& settings);
	void store() const;

	// Custom (NUIT): kept outside the settings blob so its stored layout doesn't change
	RobotConfigData getRobotConfig() const;
	void setRobotConfig(const RobotConfigData& config);

	// Custom (NUIT): VOICE (VoicePreset), so the robot speaks in it from power-on, before the controller connects
	uint8_t getVoice() const;
	void setVoice(uint8_t voice);

private:
	SettingsStruct settingsStruct;

	static constexpr const char* BlobName = "Settings";
	static constexpr const char* NVSNamespace = "Butterbot";
	nvs_handle_t handle{};

	void load();

	RobotConfigData robotConfig{ 80, 0, 40, 1 };
	static constexpr const char* VolumeKey = "Volume";
	static constexpr const char* NightModeKey = "NightMode";
	static constexpr const char* NightVolumeKey = "NightVol";
	static constexpr const char* RoamingKey = "Roaming";
	uint8_t voice = 0;
	static constexpr const char* VoiceKey = "Voice";
};


#endif //BUTTERBOT_FIRMWARE_SETTINGS_H