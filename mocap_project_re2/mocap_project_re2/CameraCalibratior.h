#pragma once

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class СameraСalibration
{
public:
	СameraСalibration(Size2i& patternSize, float cellSize);
	void StartCalibrate(vector<string>& inputPaths);

private:
	Size imageSize;
	Size2i patternSize;
	float cellSize;

	vector<Point3f> chessboard3dPoints;

	vector<vector<Point2f>> findedCorners;
	vector<vector<Point3f>> all3dPoints;

	Mat cameraMatrix;
	Mat distCoeffs;
	vector<Mat> rvecs;
	vector<Mat> tvecs;
public:
	const Mat& GetCameraMatrix() const;
	const Mat& GetDistortionMatrix() const;

};

