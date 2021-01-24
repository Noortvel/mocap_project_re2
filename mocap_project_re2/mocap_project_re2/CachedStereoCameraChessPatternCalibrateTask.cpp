#include "CachedStereoCameraChessPatternCalibrateTask.h"
#include <fstream>
#include <opencv2\opencv.hpp>
#include <spdlog/spdlog.h>

void CachedStereoCameraChessPatternCalibrateTask::Task::Execute(Input& input) {
	spdlog::debug("CachedStereoCameraChessPatternCalibrateTask started");
	std::ifstream file(input.cachedResultFilePath);
	if (file.good()) {
		file.close();
		result.Load(input.cachedResultFilePath);
		spdlog::debug("Camera calib restored from cache");
	}
	else {
		StereoCameraChessPatternCalibrateTask::Task stereoCamCalibTask;
		stereoCamCalibTask.Execute(input);
		result = std::move((const CachedStereoCameraChessPatternCalibrateTask::Result&)stereoCamCalibTask.Result());
		result.Save(input.cachedResultFilePath);
	}
	spdlog::debug("CachedStereoCameraChessPatternCalibrateTask ended");

}
void CachedStereoCameraChessPatternCalibrateTask::Result::Save(std::string& path)
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

void CachedStereoCameraChessPatternCalibrateTask::Result::Load(std::string& path)
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