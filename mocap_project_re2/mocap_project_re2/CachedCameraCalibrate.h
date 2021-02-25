#pragma once
#include "ITask.h"
#include "IFileCached.h"
#include "CameraCalibrate.h"


namespace openmocap2 {

	struct CachedCameraCalibrate {
		struct Result : public CameraCalibrate::Result, IFileCached {
			void Save(std::string& path) override;
			void Load(std::string& path) override;
		};
		void Execute();
	};

	namespace CachedCameraCalibrate
	{
		struct Input : public CameraCalibrate::Input {
			std::string cachedResultFilePath;
		};
		
		struct Task : public ITask<Input, Result> {
			void Execute(Input& input) override;
		};
	}
}

