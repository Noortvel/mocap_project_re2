#pragma once
#include "ITask.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

namespace StereoCameraChessPatternCalibrateTask
{
	using namespace cv;
	using namespace std;
	struct Input {
		Size2i patternSize;
		float cellSize;
		string cameraLCalibrateImagesPathMask;
		string cameraRCalibrateImagesPathMask;
		vector<Point3f>* chessboard3dPoints;
	};
	struct Result {
		Mat cameraMatrix0;
		Mat distCoeffs0;
		Mat cameraMatrix1;
		Mat distCoeffs1;
		Mat R, T, E, F;
	};
	struct Task : ITask<Input, Result> {
		void Execute(Input& input) override;
	private:
		void CalculatePlanarAndObjectArraysOfPoints(
			string& imagesPath,
			Size2i& pattenSize,
			vector<Point3f>& chessboard3dPoints,
			vector<vector<Point2f>>& outPlanar,
			int& objectsCount);
		Size2i imageSize;
	};
}

