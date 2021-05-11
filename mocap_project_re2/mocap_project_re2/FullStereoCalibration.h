#pragma once
#include <string>
#include "CachedCameraCalibrate.h"
#include "CachedStereoCameraCalibrate.h";
#include "StereoRectify.h"
#include "InitUndistortRectifyMap.h"

namespace openmocap2 
{
	using namespace std;

	struct FullStereoCalibration
	{
		FullStereoCalibration();
		void Calibrate();
		const InitUndistortRectifyMap::Result& InitUndistortRectifyMap1() const;
		const InitUndistortRectifyMap::Result& InitUndistortRectifyMap2() const;
		const StereoRectify::Result& Rectify() const;

	private:
		const string CAM1_CALIB_CACHE_PATH = "./data_cache/cameraCalibCommon.json";
		const string STEREO_CAM_CALIB_CACHE_PATH = "./data_cache/stereoCameraCalibCommon.json";
		CachedCameraCalibrate camera1Calibrate;
		CachedStereoCameraCalibrate cachedStereoCameraCalib;
		StereoRectify rectify;
		InitUndistortRectifyMap initUndistortRectifyMap1;
		InitUndistortRectifyMap initUndistortRectifyMap2;
	};
}