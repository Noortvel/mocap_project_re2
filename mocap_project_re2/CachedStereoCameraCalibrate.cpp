#include "CachedStereoCameraCalibrate.h"
#include <fstream>
#include <opencv2\opencv.hpp>
#include <spdlog/spdlog.h>

void openmocap2::CachedStereoCameraCalibrate::Result::Save(std::string& path)
{
	cv::FileStorage fs(path, cv::FileStorage::WRITE);
	fs << "cameraMatrix0" << cameraMatrix0;
	fs << "distCoeffs0" << distCoeffs0;
	fs << "cameraMatrix1" << cameraMatrix1;
	fs << "distCoeffs1" << distCoeffs1;
	fs << "R" << R;
	fs << "T" << T;
	fs << "E" << T;
	fs << "F" << T;

}

void openmocap2::CachedStereoCameraCalibrate::Result::Load(std::string& path)
{
	cv::FileStorage fs(path, cv::FileStorage::READ);
	fs["cameraMatrix0"] >> cameraMatrix0;
	fs ["distCoeffs0"] >> distCoeffs0;
	fs ["cameraMatrix1"] >> cameraMatrix1;
	fs ["distCoeffs1"] >> distCoeffs1;
	fs ["R"] >> R;
	fs ["T"] >> T;
	fs ["E"] >> T;
	fs ["F"] >> T;
}

void openmocap2::CachedStereoCameraCalibrate::Execute(
	const Size2i& patternSize,
	const float cellSize,
	const string& camera1StereoCalibrateImagesPathMask,
	const string& camera2StereoCalibrateImagesPathMask,
	const vector<Point3f>& chessboard3dPoints,
	const CameraCalibrate::Result& camera1Result,
	const CameraCalibrate::Result& camera2Result)
{
	spdlog::debug("CachedStereoCameraChessPatternCalibrateTask started");
	std::ifstream file(cachePath);
	if (file.good()) {
		file.close();
		result.Load(cachePath);
		spdlog::debug("Camera calib restored from cache");
	}
	else {
		StereoCameraCalibrate::Execute(
			patternSize,
			cellSize,
			camera1StereoCalibrateImagesPathMask,
			camera2StereoCalibrateImagesPathMask,
			chessboard3dPoints,
			camera1Result,
			camera2Result);
		result = std::move(static_cast<CachedStereoCameraCalibrate::Result&> (StereoCameraCalibrate::result));
		result.Save(cachePath);
	}
	spdlog::debug("CachedStereoCameraChessPatternCalibrateTask ended");
}

openmocap2::CachedStereoCameraCalibrate::CachedStereoCameraCalibrate(
	const string&& cachePath,
	const Size2i&& imageSize) : StereoCameraCalibrate(std::move(imageSize))
{
	this->cachePath = cachePath;
}
