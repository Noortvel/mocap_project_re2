#include "StereoRectify.h"
#include <opencv2/calib3d/calib3d.hpp>

void openmocap2::StereoRectify::Task::Execute(Input& input) {
	cv::stereoRectify(
		input.cameraMatrix1,
		input.distCoeffs1,
		input.cameraMatrix2,
		input.distCoeffs2,
		input.imageSize,
		input.R,
		input.T,
		result.R1, result.R2, result.P1, result.P2, result.Q,
		1024, -1, cv::Size(), &result.ROI1, &result.ROI2);
}