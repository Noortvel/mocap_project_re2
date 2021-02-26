#include "CachedCameraCalibrate.h"
#include <fstream>
#include <opencv2/opencv.hpp>
#include <spdlog/spdlog.h>

void openmocap2::CachedCameraCalibrate::Result::Save(std::string& path)
{
	cv::FileStorage fs(path, cv::FileStorage::WRITE);
	fs << "cameraMatrix" << cameraMatrix;
	fs << "distCoeffs" << distCoeffs;
	fs << "rvecs" << rvecs;
	fs << "tvecs" << tvecs;
}

void openmocap2::CachedCameraCalibrate::Result::Load(std::string& path)
{
	cv::FileStorage fs(path, cv::FileStorage::READ);
	fs["cameraMatrix"] >> cameraMatrix;
	fs["distCoeffs"] >> distCoeffs;
	fs["rvecs"] >> rvecs;
	fs["tvecs"] >> tvecs;
}

void openmocap2::CachedCameraCalibrate::Execute(
	const Size2i& patternSize, 
	const float cellSize,
	const string& calibrateImagesPathMask, 
	const vector<Point3f>& chessboard3dPoints)
{
	spdlog::debug("CachedCameraChessPatternCalibrateTask started");
	std::ifstream file(cachePath);
	if (file.good()) {
		file.close();
		result.Load(cachePath);
		spdlog::debug("Camera calib restored from cache");
	}
	else {
		CameraCalibrate::Execute(patternSize, cellSize, calibrateImagesPathMask, chessboard3dPoints);
		result = std::move(static_cast<Result&>(CameraCalibrate::result));
		result.Save(cachePath);
	}
	spdlog::debug("CachedCameraChessPatternCalibrateTask ended");
}

openmocap2::CachedCameraCalibrate::CachedCameraCalibrate(const string&& cachePath)
{
	this->cachePath = cachePath;
}
