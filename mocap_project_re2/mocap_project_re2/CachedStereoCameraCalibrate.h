#pragma once
#include "IFileCached.h"
#include "StereoCameraCalibrate.h"
#include <string>

namespace openmocap2 {
	using namespace cv;
	using namespace std;
	struct CachedStereoCameraCalibrate : public StereoCameraCalibrate, IFileCached {
		CachedStereoCameraCalibrate(const string& cachePath);
		void Execute(
			const Size2i& patternSize,
			const float cellSize,
			const string& camera1StereoCalibrateImagesPathMask,
			const string& camera2StereoCalibrateImagesPathMask,
			const vector<Point3f>& chessboard3dPoints,
			const CameraCalibrate::Result& camera1Result,
			const CameraCalibrate::Result& camera2Result);
		void Save(std::string& path) override;
		void Load(std::string& path) override;
	private:
		string cachePath;
	};
}

