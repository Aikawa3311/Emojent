#pragma once
#include "Stage.h"

class PhysicsElements::Stage;

class StageReader
{
private:
	PhysicsElements::Stage& stage;
	FilePath path;
	TOMLReader tomlr;

public:
	StageReader(PhysicsElements::Stage& stage, FilePath const& path);

	void load();
};

