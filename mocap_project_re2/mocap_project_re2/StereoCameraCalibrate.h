#pragma once
#include "ITask.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include "CameraCalibrate.h"

namespace openmocap2 {
	using namespace cv;
	using namespace std;
	struct StereoCameraCalibrate {
		virtual void Execute(
			const Size2i& patternSize,
			const float cellSize,
			const string& camera1StereoCalibrateImagesPathMask,
			const string& camera2StereoCalibrateImagesPathMask,
			const vector<Point3f>& chessboard3dPoints,
			const CameraCalibrate::Result& camera1Result,
			const CameraCalibrate::Result& camera2Result);

		struct Result {
			Mat cameraMatrix0;
			Mat distCoeffs0;
			Mat cameraMatrix1;
			Mat distCoeffs1;
			Mat R, T, E, F;
		};
		Result result;
	private:
		Size2i imageSize;
	private:
		void CalculatePlanarAndObjectArraysOfPoints(
			const string& imagesPath,
			const Size2i& pattenSize,
			const vector<Point3f>& chessboard3dPoints,
			vector<vector<Point2f>>& outPlanar,
			int& outObjectsCount);

	};
}