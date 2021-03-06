#pragma once
#include "IFileCached.h"
#include "CameraCalibrate.h"


namespace openmocap2 {

	struct CachedCameraCalibrate : public CameraCalibrate, IFileCached {
		CachedCameraCalibrate(const string& cachePath);
		void Execute(
			const Size2i& patternSize,
			const float cellSize,
			const string& calibrateImagesPathMask,
			const vector<Point3f>& chessboard3dPoints) override;
		void Save(string& path) override;
		void Load(string& path) override;
	private:
		string cachePath;
	};
}

