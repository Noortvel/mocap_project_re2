#include "StereoCameraChessPatternCalibrateTask.h"
#include <spdlog/spdlog.h>


void StereoCameraChessPatternCalibrateTask::Task::Execute(Input& input) {
	spdlog::debug("StereoCameraChessPatternCalibrateTask started");
	vector<vector<Point2f>> planarPointsCam0;
	vector<vector<Point2f>> planarPointsCam1;

	int objectPointsCountCam0 = 0;
	int objectPointsCountCam1 = 0;

	CalculatePlanarAndObjectArraysOfPoints(
		input.cameraLCalibrateImagesPathMask,
		input.patternSize,
		*input.chessboard3dPoints,
		planarPointsCam0,
		objectPointsCountCam0);
	CalculatePlanarAndObjectArraysOfPoints(
		input.cameraRCalibrateImagesPathMask,
		input.patternSize,
		*input.chessboard3dPoints,
		planarPointsCam1,
		objectPointsCountCam1);
	int minObjPointsCount = std::min(objectPointsCountCam0, objectPointsCountCam1);
	vector<vector<Point3f>> objectPoints;
	for (int i = 0; i < objectPointsCountCam0 - minObjPointsCount; i++) {
		planarPointsCam0.pop_back();
	}
	for (int i = 0; i < objectPointsCountCam1 - minObjPointsCount; i++) {
		planarPointsCam1.pop_back();
	}
	for (int i = 0; i < minObjPointsCount; i++) {
		objectPoints.push_back(*input.chessboard3dPoints);
	}
	result.imageSize = imageSize;
	spdlog::debug("Begin stereo calibrate");
	cv::stereoCalibrate(
		objectPoints,
		planarPointsCam0,
		planarPointsCam1,
		input.camera1Result->cameraMatrix,
		input.camera1Result->distCoeffs,
		input.camera2Result->cameraMatrix,
		input.camera2Result->distCoeffs,
		imageSize,
		result.R,
		result.T,
		result.E,
		result.F, CALIB_FIX_INTRINSIC
	);
	spdlog::debug("Ended stereo calibrate");
	spdlog::debug("StereoCameraChessPatternCalibrateTask ended");
}

void StereoCameraChessPatternCalibrateTask::Task::CalculatePlanarAndObjectArraysOfPoints(
	string& imagesPath,
	Size2i& pattenSize,
	vector<Point3f>& chessboard3dPoints,
	vector<vector<Point2f>>& outPlanar,
	int& objectsCount)
{
	vector<string> inputPaths;
	glob(imagesPath, inputPaths);

	spdlog::debug("Chessboard find begin");
	int chessboardIndex = 0;
	for (auto&& imageName : inputPaths) {
		chessboardIndex++;
		vector<Point2f> corners;
		auto imageMat = imread(imageName, IMREAD_COLOR);
		imageSize.height = imageMat.rows;
		imageSize.width = imageMat.cols;

		bool succes = findChessboardCorners(imageMat, pattenSize, corners);
		spdlog::debug("Chessboard [{0}/{1}] is {2}", chessboardIndex, inputPaths.size(), succes);

		Mat grayscale;
		cv::cvtColor(imageMat, grayscale, ColorConversionCodes::COLOR_BGR2GRAY);
		if (succes) {
			cornerSubPix(grayscale, corners, Size(11, 11), Size(-1, -1), TermCriteria((TermCriteria::EPS + TermCriteria::COUNT), 30, 0.001));
			outPlanar.push_back(corners);
			objectsCount++;
			/*for (auto&& position : corners) {
				drawMarker(imageMat, position, drawColor);
				imshow(imageName, imageMat);
				cv::waitKey(0);
			}*/
			/*cout << "isFound: " << succes << " Corners Count: " << corners.size() << endl;
			imshow(imageName, imageMat);
			cv::waitKey(0);*/
		}
	}
	spdlog::debug("Chessboard find end");
}

