#include "InitUndistortRectifyMap.h"
#include "CameraCalibrate.h"

void openmocap2::InitUndistortRectifyMap::Task::Execute(Input& input) {
	cv::initUndistortRectifyMap(
		input.cameraMatrix,
		input.distCoeffs,
		input.R, input.P,
		input.imageSize,
		CV_32F,
		result.map1,
		result.map2);
}

openmocap2::InitUndistortRectifyMap::Input::Input()
{
}
openmocap2::InitUndistortRectifyMap::Input::Input(CameraCalibrate::Result& camCalibResult, StereoRectify::Result& stereoRectifyResult)
{

}
