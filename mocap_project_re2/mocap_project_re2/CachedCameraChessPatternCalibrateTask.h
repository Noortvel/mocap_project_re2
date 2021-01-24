#pragma once
#include "ITask.h"
#include "IFileCached.h"
#include "CameraChessPatternCalibrateTask.h"


namespace CachedCameraChessPatternCalibrateTask
{
	struct Input : CameraChessPatternCalibrateTask::Input {
		std::string cachedResultFilePath;
	};
	struct Result : public CameraChessPatternCalibrateTask::Result, IFileCached {
		void Save(std::string& path) override;
		void Load(std::string& path) override;
	};
	struct Task : public ITask<Input, Result> {
		void Execute(Input& input) override;
	};
};

