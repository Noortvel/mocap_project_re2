#pragma once
#include "IFileCached.h"
#include "StereoCameraChessPatternCalibrateTask.h"
#include <string>

namespace CachedStereoCameraChessPatternCalibrateTask
{
	struct Input : public StereoCameraChessPatternCalibrateTask::Input {
		std::string cachedResultFilePath;
	};
	struct Result : public StereoCameraChessPatternCalibrateTask::Result, IFileCached {
		void Save(std::string& path) override;
		void Load(std::string& path) override;
	};
	struct Task : ITask<Input, Result>{
		void Execute(Input& input) override;
	};
}

