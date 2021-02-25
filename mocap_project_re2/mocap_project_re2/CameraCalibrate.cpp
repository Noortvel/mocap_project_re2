#include "CameraCalibrate.h"
#include <spdlog/spdlog.h>
#include <fstream>

void openmocap2::CameraCalibrate::Execute(
	Size2i& patternSize,
	float cellSize,
	string& calibrateImagesPathMask,
	vector<Point3f>& chessboard3dPoints)
{
	spdlog::debug("CameraCalibrate started");
	if (isCached) {
		if (isLoaded) return;
		std::ifstream file(cachePath);
		if (file.good()) {
			file.close();
			result.Load(cachePath);
			isLoaded = true;
			spdlog::debug("Camera calib restored from cache");
		}
	}
	vector<vector<Point2f>> findedCorners;
	vector<vector<Point3f>> all3dPoints;

	Size2i imageSize;
	vector<string> inputPaths;
	glob(calibrateImagesPathMask, inputPaths);

	spdlog::debug("Chessboard find begin");
	int chessboardIndex = 0;
	for (auto&& imageName : inputPaths) {
		chessboardIndex++;
		vector<Point2f> corners;
		auto imageMat = imread(imageName, IMREAD_COLOR);
		imageSize.height = imageMat.rows;
		imageSize.width = imageMat.cols;

		bool succes = cv::findChessboardCorners(imageMat, patternSize, corners);
		//cout << imageName << " Chessboard [" << chessboardIndex << "/" << inputPaths.size() << "] " << succes << endl;
		spdlog::debug("Chessboard [{0}/{1}] is {2}", chessboardIndex, inputPaths.size(), succes);

		Mat grayscale;
		cv::cvtColor(imageMat, grayscale, ColorConversionCodes::COLOR_BGR2GRAY);
		if (succes) {
			cv::cornerSubPix(grayscale, corners, Size(11, 11), Size(-1, -1), TermCriteria((TermCriteria::EPS + TermCriteria::COUNT), 30, 0.001));
			findedCorners.push_back(corners);
			all3dPoints.push_back(chessboard3dPoints);
			//for (auto&& position : corners) {
			//	drawMarker(imageMat, position, cv::Scalar(53, 171, 245, 0));
			//	//imshow(imageName, imageMat);
			//	//cv::waitKey(0);
			//}
			/*cout << "isFound: " << succes << " Corners Count: " << corners.size() << endl;
			imshow(imageName, imageMat);
			cv::waitKey(0);*/
		}
	}

	spdlog::debug("Chessboard find end");
	spdlog::debug("Calibrate camera begin");
	cv::calibrateCamera(all3dPoints, findedCorners, imageSize, result.cameraMatrix, result.distCoeffs, result.rvecs, result.tvecs, 0, TermCriteria((TermCriteria::EPS + TermCriteria::COUNT), 30, 0.001));
	spdlog::debug("Calibrate camera end");
	if (isCached) {
		result.Save(cachePath);
	}
	spdlog::debug("CameraCalibrate ended");
}

openmocap2::CameraCalibrate::CameraCalibrate()
{
}

openmocap2::CameraCalibrate::CameraCalibrate(string& cachePath)
{
	this->cachePath = cachePath;
	isCached = true;
}

void openmocap2::CameraCalibrate::Result::Save(std::string& path)
{
	cv::FileStorage fs(path, cv::FileStorage::WRITE);
	fs << "cameraMatrix" << cameraMatrix;
	fs << "distCoeffs" << distCoeffs;
	fs << "rvecs" << rvecs;
	fs << "tvecs" << tvecs;
}

void openmocap2::CameraCalibrate::Result::Load(std::string& path)
{
	cv::FileStorage fs(path, cv::FileStorage::READ);
	fs["cameraMatrix"] >> cameraMatrix;
	fs["distCoeffs"] >> distCoeffs;
	fs["rvecs"] >> rvecs;
	fs["tvecs"] >> tvecs;
}
