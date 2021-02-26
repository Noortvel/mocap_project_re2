#pragma once
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "IFileCached.h"

namespace openmocap2 {
	using namespace cv;
	using namespace std;
	struct CameraCalibrate {
		struct Result : public IFileCached {
			Mat cameraMatrix;
			Mat distCoeffs;
			vector<Mat> rvecs;
			vector<Mat> tvecs;
			void Save(std::string& path) override;
			void Load(std::string& path) override;
		};
		Result result;
		virtual void Execute(
			const Size2i& patternSize,
			const float cellSize,
			const string& calibrateImagesPathMask,
			const vector<Point3f>& chessboard3dPoints);
	};
}