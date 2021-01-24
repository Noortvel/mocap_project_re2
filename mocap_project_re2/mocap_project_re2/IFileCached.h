#pragma once
#include <string>

struct IFileCached {
	virtual void Save(std::string& path) = 0;
	virtual void Load(std::string& path) = 0;
};
