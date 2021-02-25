#pragma once
#include "ITask.h"
#include <opencv2/opencv.hpp>
#include "StereoRectify.h"

namespace openmocap2 {
	namespace InitUndistortRectifyMap {
		struct Input {
			Input();
			Input(
				CameraCalibrate::Result& camCalibResult,
				StereoRectify::Result& stereoRectifyResult);
			cv::Mat cameraMatrix, distCoeffs, R, P;
			cv::Size2i imageSize;
		};
		struct Result
		{
			cv::Mat map1, map2;
		};
		struct Task : ITask<Input, Result> {
			void Execute(Input& input) override;
		};
	}
}