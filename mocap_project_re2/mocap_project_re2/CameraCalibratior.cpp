#include "CameraCalibratior.h"

СameraСalibration::СameraСalibration(Size2i& patternSize, float cellSize)
{
	this->patternSize = patternSize;
	this->cellSize = cellSize;
	for (int y = patternSize.height - 1; y >= 0; y--) {
		for (int x = 0; x < patternSize.width; x++) {
			chessboard3dPoints.push_back(Point3f(x * cellSize, y * cellSize, 0.0f));
		}
	}
}

void СameraСalibration::StartCalibrate(vector<string>& inputPaths)
{
	cout << "Chessboard find begin" << endl;
	int chessboardIndex = 0;
	for (auto&& imageName : inputPaths) {
		chessboardIndex++;
		vector<Point2f> corners;
		auto imageMat = imread(imageName, IMREAD_COLOR);
		imageSize.height = imageMat.rows;
		imageSize.width = imageMat.cols;

		bool succes = findChessboardCorners(imageMat, patternSize, corners);
		cout << imageName << " Chessboard [" << chessboardIndex << "/" << inputPaths.size() << "] " << succes << endl;

		Mat grayscale;
		cv::cvtColor(imageMat, grayscale, ColorConversionCodes::COLOR_BGR2GRAY);
		if (succes) {
			cornerSubPix(grayscale, corners, Size(11, 11), Size(-1, -1), TermCriteria((TermCriteria::EPS + TermCriteria::COUNT), 30, 0.001));
			findedCorners.push_back(corners);
			all3dPoints.push_back(chessboard3dPoints);
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
	cout << "Chessboard find end" << endl;
	cout << "Calibrate camera begin" << endl;

	calibrateCamera(all3dPoints, findedCorners, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, 0, TermCriteria((TermCriteria::EPS + TermCriteria::COUNT), 30, 0.001));
	cout << "Calibrate camera end" << endl;
}

const Mat& СameraСalibration::GetCameraMatrix() const
{
	return cameraMatrix;
}

const Mat& СameraСalibration::GetDistortionMatrix() const
{
	return distCoeffs;
}
