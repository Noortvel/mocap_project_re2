#pragma once
#include "json\json_struct.h"

struct Vec3 {
	float x;
	float y;
	float z;
	JS_OBJ(x, y, z);
};