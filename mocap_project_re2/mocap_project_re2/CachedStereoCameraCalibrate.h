#pragma once
#include "IFileCached.h"
#include "StereoCameraCalibrate.h"
#include <string>

namespace openmocap2 {
	namespace CachedStereoCameraCalibrate
	{
		struct Input : public StereoCameraCalibrate::Input {
			std::string cachedResultFilePath;
		};
		struct Result : public StereoCameraCalibrate::Result, IFileCached {
			void Save(std::string& path) override;
			void Load(std::string& path) override;
		};
		struct Task : ITask<Input, Result> {
			void Execute(Input& input) override;
		};
	}
}

