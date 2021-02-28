#include "StereoRectify.h"
#include <opencv2/calib3d/calib3d.hpp>

void openmocap2::StereoRectify::Execute(
	const Size2i& imageSize,
	const StereoCameraCalibrate::Result& scc)
{
	cv::stereoRectify(
		scc.cameraMatrix0,
		scc.distCoeffs0,
		scc.cameraMatrix1,
		scc.distCoeffs1,
		imageSize,
		scc.R,
		scc.T,
		result.R1, result.R2, result.P1, result.P2, result.Q);
		//1024, -1, cv::Size(), &result.ROI1, &result.ROI2);
}
