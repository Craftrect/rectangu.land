#pragma once

namespace Terrain
{
	class TerrainNoise : public Singleton<TerrainNoise>
	{
	private:
		Utils::PerlinNoise mHeightNoise;

	public:
		TerrainNoise();
		uint32 sampleHeight(float x, float y);
	};
}

#define sTerrainNoise (Terrain::TerrainNoise::getInstance())