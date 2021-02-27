#pragma once
#include "ITask.h"
#include <opencv2/opencv.hpp>
#include "StereoRectify.h"

namespace openmocap2 {
	using namespace cv;
	struct InitUndistortRectifyMap {
		void Execute(
			const Size2i& imageSize,
			const Mat& cameraMatrix,
			const Mat& distCoeffs,
			const Mat& R,
			const Mat& P);
		struct Result
		{
			Mat map1, map2;
		};
		Result result;
		
	};
}