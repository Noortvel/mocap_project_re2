#pragma once
#include "ITask.h"
#include "IFileCached.h"
#include "CameraCalibrate.h"


namespace openmocap2 {

	struct CachedCameraCalibrate : public CameraCalibrate {
		struct Result : public CameraCalibrate::Result, IFileCached {
			void Save(string& path) override;
			void Load(string& path) override;
		};
		void Execute(
			const Size2i& patternSize,
			const float cellSize,
			const string& calibrateImagesPathMask,
			const vector<Point3f>& chessboard3dPoints) override;
		CachedCameraCalibrate(const string&& cachePath);
	private:
		string cachePath;
	};
}

