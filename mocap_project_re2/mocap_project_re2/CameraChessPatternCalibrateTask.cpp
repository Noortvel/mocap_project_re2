#include "CameraCalibrate.h"
#include <spdlog/spdlog.h>

void openmocap2::CameraCalibrate::Task::Execute(Input& input)
{
	spdlog::debug("CameraChessPatternCalibrateTask started");
	vector<vector<Point2f>> findedCorners;
	vector<vector<Point3f>> all3dPoints;

	Size2i imageSize;
	vector<string> inputPaths;
	glob(input.calibrateImagesPathMask, inputPaths);

	spdlog::debug("Chessboard find begin");
	int chessboardIndex = 0;
	for (auto&& imageName : inputPaths) {
		chessboardIndex++;
		vector<Point2f> corners;
		auto imageMat = imread(imageName, IMREAD_COLOR);
		imageSize.height = imageMat.rows;
		imageSize.width = imageMat.cols;

		bool succes = cv::findChessboardCorners(imageMat, input.patternSize, corners);
		//cout << imageName << " Chessboard [" << chessboardIndex << "/" << inputPaths.size() << "] " << succes << endl;
		spdlog::debug("Chessboard [{0}/{1}] is {2}", chessboardIndex, inputPaths.size(), succes);

		Mat grayscale;
		cv::cvtColor(imageMat, grayscale, ColorConversionCodes::COLOR_BGR2GRAY);
		if (succes) {
			cv::cornerSubPix(grayscale, corners, Size(11, 11), Size(-1, -1), TermCriteria((TermCriteria::EPS + TermCriteria::COUNT), 30, 0.001));
			findedCorners.push_back(corners);
			all3dPoints.push_back(*input.chessboard3dPoints);
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

	spdlog::debug("CameraChessPatternCalibrateTask ended");
}
