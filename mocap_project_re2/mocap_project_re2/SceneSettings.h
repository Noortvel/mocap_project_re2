#pragma once

#include <vector>
#include "ISerializable.h"
#include "json\json_struct.h"

using namespace std;

class SceneSettings
{
private:
	vector<vector<int>> camera_pairs;
	vector<double> distanes_between_cameras;
public:
	const vector<vector<int>>& GetCameraPairs() const;
	const vector<double>& GetDistanesBetweenCameras() const;
	JS_OBJ(camera_pairs, distanes_between_cameras);
};

