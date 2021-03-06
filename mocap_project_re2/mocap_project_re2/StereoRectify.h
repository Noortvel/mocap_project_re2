#pragma once
#include <opencv2\opencv.hpp>
#include <vector>
#include "StereoCameraCalibrate.h"

namespace openmocap2 {
	using namespace cv;
	using namespace std;
	struct StereoRectify {
		void Execute(
			const Size2i& imageSize,
			const StereoCameraCalibrate::Result& scc);
		struct Result
		{
			Mat R1, R2, P1, P2, Q;
			//Rect ROI1, ROI2;
		};
		Result result;
	};
}