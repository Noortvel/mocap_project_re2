#include "SceneSettings.h"

const vector<vector<int>>& SceneSettings::GetCameraPairs() const
{
    return camera_pairs;
}

const vector<double>& SceneSettings::GetDistanesBetweenCameras() const
{
    return distanes_between_cameras;
}
