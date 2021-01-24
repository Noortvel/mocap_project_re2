#include "MainSettings.h"

const SceneSettings& MainSettings::GetCameraSettings() const
{
	return camera_settings;
}

void MainSettings::LoadConfigs()
{
	camera_settings = LoadObjectFromFile<SceneSettings>("./data/calibration/cameras_settings.json");
}
