#include "stdafx.h"
#include "StageReader.h"

StageReader::StageReader(PhysicsElements::Stage& stage, FilePath const& path)
	: stage(stage),
	path(path),
	tomlr(path)
{
	if (!path) {
		throw Error{ U"Failed to load `{}`"_fmt(path) };
	}
}

void StageReader::load()
{
	if (!tomlr.isOpen()) {
		tomlr.open(path);
	}

	if (!path) {
		throw Error{ U"Failed to load `{}`"_fmt(path) };
	}
}


