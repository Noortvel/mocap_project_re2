#pragma once
#include "ITask.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

namespace openmocap2 {
	namespace CameraCalibrate {
		using namespace cv;
		using namespace std;
		struct Input {
			Size2i patternSize;
			float cellSize;
			string calibrateImagesPathMask;
			vector<Point3f>* chessboard3dPoints;
		};
		struct Result {
			Mat cameraMatrix;
			Mat distCoeffs;
			vector<Mat> rvecs;
			vector<Mat> tvecs;
		};
		struct Task : ITask<Input, Result> {
			void Execute(Input& input) override;
		};
	}
}