#include "CachedCameraCalibrate.h"
#include <fstream>
#include <opencv2\opencv.hpp>
#include <spdlog/spdlog.h>


void openmocap2::CachedCameraCalibrate::Task::Execute(Input& input) {
	spdlog::debug("CachedCameraChessPatternCalibrateTask started");
	std::ifstream file(input.cachedResultFilePath);
	if (file.good()) {
		file.close();
		result.Load(input.cachedResultFilePath);
		spdlog::debug("Camera calib restored from cache");
	}
	else {
		openmocap2::CameraCalibrate::Task calibrateCameraCommonTask;
		calibrateCameraCommonTask.Execute(input);
		result = std::move((const CachedCameraCalibrate::Result&)calibrateCameraCommonTask.Result());
		result.Save(input.cachedResultFilePath);
	}
	spdlog::debug("CachedCameraChessPatternCalibrateTask ended");
}

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
