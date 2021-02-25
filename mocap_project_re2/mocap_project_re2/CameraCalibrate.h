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
		void Execute(
			Size2i& patternSize,
			float cellSize,
			string& calibrateImagesPathMask,
			vector<Point3f>& chessboard3dPoints);
		CameraCalibrate();
		CameraCalibrate(string& cachePath);
	private:
		string cachePath;
		bool isCached = false;
		bool isLoaded = false;
	};

	namespace CameraCalibrate123 {
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