#pragma once
#include <fstream>
#include <string>
#include "SceneSettings.h"

class MainSettings
{
private:
	template<typename T>
	T LoadObjectFromFile(string&& path)
	{
		using namespace std;
		ifstream json_file(path);
		string json_string((istreambuf_iterator<char>(json_file)), istreambuf_iterator<char>());
		JS::ParseContext context(json_string);
		T obj;
		context.parseTo(obj);
		return obj;
	}
private:
	SceneSettings camera_settings;
public:
	const SceneSettings& GetCameraSettings() const;
	void LoadConfigs();
};

