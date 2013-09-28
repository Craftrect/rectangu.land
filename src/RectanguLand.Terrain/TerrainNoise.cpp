#include "stdafx.h"
#include "TerrainNoise.h"

INIT_SINGLETON(Terrain::TerrainNoise);

namespace Terrain
{
	TerrainNoise::TerrainNoise() {
		mHeightNoise.setPersistence(0.65f);
		mHeightNoise.setOctaves(4);
	}

	uint32 TerrainNoise::sampleHeight(float x, float y) {
		float valNorm = mHeightNoise.sample(x, y);
		valNorm = (valNorm + 1) / 2.0f;

		return (uint32) (valNorm * 30);
	}

	
}