#pragma once
#include <vector>
#include "Keypoint.h"

namespace openmocap2 {
	using namespace std;
	class MotionCapter
	{
	public:
		void Start(vector<vector<Keypoint>>& animation);
	};
}

