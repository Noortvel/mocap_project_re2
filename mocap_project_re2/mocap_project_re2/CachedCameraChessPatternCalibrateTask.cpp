#include "CachedCameraChessPatternCalibrateTask.h"
#include <fstream>
#include <opencv2\opencv.hpp>
#include <spdlog/spdlog.h>


void CachedCameraChessPatternCalibrateTask::Task::Execute(Input& input) {
	spdlog::debug("CachedCameraChessPatternCalibrateTask started");
	std::ifstream file(input.cachedResultFilePath);
	if (file.good()) {
		file.close();
		result.Load(input.cachedResultFilePath);
		spdlog::debug("Camera calib restored from cache");
	}
	else {
		CameraChessPatternCalibrateTask::Task calibrateCameraCommonTask;
		calibrateCameraCommonTask.Execute(input);
		result = std::move((const CachedCameraChessPatternCalibrateTask::Result&)calibrateCameraCommonTask.Result());
		result.Save(input.cachedResultFilePath);
	}
	spdlog::debug("CachedCameraChessPatternCalibrateTask ended");
}

void CachedCameraChessPatternCalibrateTask::Result::Save(std::string& path)
{
	cv::FileStorage fs(path, cv::FileStorage::WRITE);
	fs << "cameraMatrix" << cameraMatrix;
	fs << "distCoeffs" << distCoeffs;
	fs << "rvecs" << rvecs;
	fs << "tvecs" << tvecs;
}

void CachedCameraChessPatternCalibrateTask::Result::Load(std::string& path)
{
	cv::FileStorage fs(path, cv::FileStorage::READ);
	fs["cameraMatrix"] >> cameraMatrix;
	fs["distCoeffs"] >> distCoeffs;
	fs["rvecs"] >> rvecs;
	fs["tvecs"] >> tvecs;
}
