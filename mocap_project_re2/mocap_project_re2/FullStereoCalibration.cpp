#include "FullStereoCalibration.h"
#include "CachedCameraCalibrate.h"
#include "CachedStereoCameraCalibrate.h"
#include "StereoRectify.h"
#include "InitUndistortRectifyMap.h"

openmocap2::FullStereoCalibration::FullStereoCalibration():
	camera1Calibrate(CAM1_CALIB_CACHE_PATH),
	cachedStereoCameraCalib(STEREO_CAM_CALIB_CACHE_PATH)
{

}

void openmocap2::FullStereoCalibration::Calibrate()
{
	auto patternSize = Size2i(6, 9);
	auto cellSize = 2 * 0.025226f;//
	vector<Point3f> chessboardPoints3D;
	for (int y = patternSize.height - 1; y >= 0; y--) {
		for (int x = 0; x < patternSize.width; x++) {
			chessboardPoints3D.push_back(Point3f(x * cellSize, y * cellSize, 0.0f));
		}
	}
	Mat _calibImg = imread("./data/calibration/common/StereoCamera0-1611526867375.png");
	auto _imgSize = _calibImg.size();

	//For better calib result every camera must be calibrate by they own
	camera1Calibrate.Execute(
		patternSize,
		cellSize,
		"./data/calibration/inner_params/camera_common/Callibration-CameraCommon*.png",
		chessboardPoints3D);

	//For syntatic data used 2 equals cameras
	cachedStereoCameraCalib.Execute(
		patternSize,
		cellSize,
		"./data/calibration/common/StereoCamera0*.png",
		"./data/calibration/common/StereoCamera1*.png",
		chessboardPoints3D,
		camera1Calibrate.result,
		camera1Calibrate.result);


	rectify.Execute(_imgSize, cachedStereoCameraCalib.result);

	initUndistortRectifyMap1.Execute(
		_imgSize,
		camera1Calibrate.result.cameraMatrix,
		camera1Calibrate.result.distCoeffs,
		rectify.result.R1,
		rectify.result.P1);
	initUndistortRectifyMap2.Execute(
		_imgSize,
		camera1Calibrate.result.cameraMatrix,
		camera1Calibrate.result.distCoeffs,
		rectify.result.R2,
		rectify.result.P2);
}

const openmocap2::InitUndistortRectifyMap::Result& openmocap2::FullStereoCalibration::InitUndistortRectifyMap1() const
{
	return initUndistortRectifyMap1.result;
}

const openmocap2::InitUndistortRectifyMap::Result& openmocap2::FullStereoCalibration::InitUndistortRectifyMap2() const
{
	return initUndistortRectifyMap2.result;
}

const openmocap2::StereoRectify::Result& openmocap2::FullStereoCalibration::Rectify() const
{
	return rectify.result;
}
