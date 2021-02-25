#pragma once
#include "ITask.h"
#include <opencv2\opencv.hpp>
#include <vector>
#include "StereoCameraCalibrate.h"

namespace openmocap2 {
	namespace StereoRectify {
		struct Input {
			Input();
			Input(cv::Size2i& imageSize, const StereoCameraCalibrate::Result& stCamCalibResult);
			cv::Size2i imageSize;
			cv::Mat cameraMatrix1, distCoeffs1;
			cv::Mat cameraMatrix2, distCoeffs2;
			cv::Mat R, T;
		};
		struct Result
		{
			cv::Mat R1, R2, P1, P2, Q;
			cv::Rect ROI1, ROI2;
		};
		struct Task : ITask<Input, Result>
		{
			void Execute(Input& input) override;
		};
	}
}