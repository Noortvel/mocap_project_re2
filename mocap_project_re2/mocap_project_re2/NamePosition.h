#pragma once
#include <string>
#include <vector>
#include <cmath>

struct Vec3 {
	float x;
	float y;
	float z;
	Vec3 operator-(Vec3& v) {
		Vec3 result = *this;
		result.x -= v.x;
		result.y -= v.y;
		result.z -= v.z;
		return result;
	}
	float length() {
		return std::sqrt(x * x + y * y + z * z);
	}
};

struct NamePosition
{
	std::string Name;
	std::vector<Vec3> Position;
};
