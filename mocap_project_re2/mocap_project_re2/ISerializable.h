#pragma once
#include <string>

class ISerializable {
public:
	virtual void FromJson(const std::string& json) = 0;
	virtual std::string ToJson() = 0;
};
