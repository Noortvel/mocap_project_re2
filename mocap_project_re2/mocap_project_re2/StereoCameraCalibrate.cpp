#include "StereoCameraCalibrate.h"
#include <spdlog/spdlog.h>

void openmocap2::StereoCameraCalibrate::CalculatePlanarAndObjectArraysOfPoints(
	const string& imagesPath,
	const Size2i& pattenSize,
	const vector<Point3f>& chessboard3dPoints,
	vector<vector<Point2f>>& outPlanar,
	int& outObjectsCount)
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
			outObjectsCount++;
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

void openmocap2::StereoCameraCalibrate::Execute(
	const Size2i& patternSize,
	const float cellSize,
	const string& camera1StereoCalibrateImagesPathMask,
	const string& camera2StereoCalibrateImagesPathMask,
	const vector<Point3f>& chessboard3dPoints,
	const CameraCalibrate::Result& camera1Result,
	const CameraCalibrate::Result& camera2Result)
{
	spdlog::debug("Begin StereoCameraCalibrate::Execute");
	vector<vector<Point2f>> planarPointsCam0;
	vector<vector<Point2f>> planarPointsCam1;

	int objectPointsCountCam0 = 0;
	int objectPointsCountCam1 = 0;

	CalculatePlanarAndObjectArraysOfPoints(
		camera1StereoCalibrateImagesPathMask,
		patternSize,
		chessboard3dPoints,
		planarPointsCam0,
		objectPointsCountCam0);
	CalculatePlanarAndObjectArraysOfPoints(
		camera2StereoCalibrateImagesPathMask,
		patternSize,
		chessboard3dPoints,
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
		objectPoints.push_back(chessboard3dPoints);
	}
	spdlog::debug("Begin cv::stereoCalibrate");
	cv::stereoCalibrate(
		objectPoints,
		planarPointsCam0,
		planarPointsCam1,
		camera1Result.cameraMatrix,
		camera1Result.distCoeffs,
		camera2Result.cameraMatrix,
		camera2Result.distCoeffs,
		imageSize,
		result.R,
		result.T,
		result.E,
		result.F, CALIB_FIX_INTRINSIC
	);
	result.cameraMatrix0 = camera1Result.cameraMatrix;
	result.distCoeffs0 = camera1Result.distCoeffs;
	result.cameraMatrix1 = camera2Result.cameraMatrix;
	result.distCoeffs1 = camera2Result.distCoeffs;
	spdlog::debug("Ended cv::stereoCalibrate");
	spdlog::debug("Ended StereoCameraCalibrate::Execute");
}
