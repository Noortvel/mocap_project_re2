#include "InitUndistortRectifyMap.h"
#include "CameraCalibrate.h"


void openmocap2::InitUndistortRectifyMap::Execute(
	const Size2i& imageSize,
	const Mat& cameraMatrix,
	const Mat& distCoeffs,
	const Mat& R,
	const Mat& P) 
{
	cv::initUndistortRectifyMap(
		cameraMatrix,
		distCoeffs,
		R, P,
		imageSize,
		CV_32F,
		result.map1,
		result.map2);
}