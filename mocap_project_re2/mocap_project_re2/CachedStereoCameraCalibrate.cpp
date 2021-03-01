#include "CachedStereoCameraCalibrate.h"
#include <fstream>
#include <opencv2\opencv.hpp>
#include <spdlog/spdlog.h>

void openmocap2::CachedStereoCameraCalibrate::Save(std::string& path)
{
	cv::FileStorage fs(path, cv::FileStorage::WRITE);
	fs << "cameraMatrix0" << result.cameraMatrix0;
	fs << "distCoeffs0" << result.distCoeffs0;
	fs << "cameraMatrix1" << result.cameraMatrix1;
	fs << "distCoeffs1" << result.distCoeffs1;
	fs << "R" << result.R;
	fs << "T" << result.T;
	fs << "E" << result.E;
	fs << "F" << result.F;

}

void openmocap2::CachedStereoCameraCalibrate::Load(std::string& path)
{
	cv::FileStorage fs(path, cv::FileStorage::READ);
	fs ["cameraMatrix0"] >> result.cameraMatrix0;
	fs ["distCoeffs0"] >> result.distCoeffs0;
	fs ["cameraMatrix1"] >> result.cameraMatrix1;
	fs ["distCoeffs1"] >> result.distCoeffs1;
	fs ["R"] >> result.R;
	fs ["T"] >> result.T;
	fs ["E"] >> result.E;
	fs ["F"] >> result.F;
	cout << "cameraMatrix0" << result.cameraMatrix0 << endl;
	cout << "distCoeffs0" << result.distCoeffs0 << endl;
	cout << "cameraMatrix1" << result.cameraMatrix1 << endl;
	cout << "distCoeffs1" << result.distCoeffs1 << endl;
	cout << "R" << result.R << endl;
	cout << "T" << result.T << endl;
	cout << "E" << result.E << endl;
	cout << "F" << result.F << endl;
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
		Load(cachePath);
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
		Save(cachePath);
	}
	spdlog::debug("CachedStereoCameraChessPatternCalibrateTask ended");
}

openmocap2::CachedStereoCameraCalibrate::CachedStereoCameraCalibrate(
	const string& cachePath)
{
	this->cachePath = cachePath;
}
