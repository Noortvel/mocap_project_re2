#include "CachedCameraCalibrate.h"
#include <fstream>
#include <opencv2/opencv.hpp>
#include <spdlog/spdlog.h>

void openmocap2::CachedCameraCalibrate::Save(std::string& path)
{
	cv::FileStorage fs(path, cv::FileStorage::WRITE);
	fs << "cameraMatrix" << result.cameraMatrix;
	fs << "distCoeffs" << result.distCoeffs;
	fs << "rvecs" << result.rvecs;
	fs << "tvecs" << result.tvecs;
}

void openmocap2::CachedCameraCalibrate::Load(std::string& path)
{
	cv::FileStorage fs(path, cv::FileStorage::READ);
	fs["cameraMatrix"] >> result.cameraMatrix;
	fs["distCoeffs"] >> result.distCoeffs;
	fs["rvecs"] >> result.rvecs;
	fs["tvecs"] >> result.tvecs;
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
		Load(cachePath);
		spdlog::debug("Camera calib restored from cache");
	}
	else {
		CameraCalibrate::Execute(patternSize, cellSize, calibrateImagesPathMask, chessboard3dPoints);
		Save(cachePath);
	}
	spdlog::debug("CachedCameraChessPatternCalibrateTask ended");
}

openmocap2::CachedCameraCalibrate::CachedCameraCalibrate(const string& cachePath)
{
	this->cachePath = cachePath;
}
