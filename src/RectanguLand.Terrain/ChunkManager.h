#pragma once

#include "MapChunk.h"

namespace Terrain
{
	class ChunkManager : public Singleton<ChunkManager>
	{
		std::map<int32, std::map<int32, MapChunkPtr>> mChunkMap;

	public:
		bool getHeightByCoord(int32 x, int32 y, uint32& z);
		void addChunk(MapChunkPtr chunk);
		void updateLights();
	};
}

#define sChunkMgr (Terrain::ChunkManager::getInstance())