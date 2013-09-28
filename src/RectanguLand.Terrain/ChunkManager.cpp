#include "stdafx.h"
#include "ChunkManager.h"

INIT_SINGLETON(Terrain::ChunkManager);

namespace Terrain
{
	void ChunkManager::addChunk(MapChunkPtr chunk) {
		mChunkMap[chunk->getIndexX()][chunk->getIndexY()] = chunk;
	}

	bool ChunkManager::getHeightByCoord(int32 x, int32 y, uint32& z) {
		int32 ix = abs(x) / 16;
		int32 iy = abs(y) / 16;

		if (x < 0) {
			ix *= -1;
		}

		if (y < 0) {
			iy *= -1;
		}

		auto xmap = mChunkMap.find(ix);
		if (xmap == mChunkMap.end()) {
			return false;
		}

		auto itr = xmap->second.find(iy);
		if (itr == xmap->second.end()) {
			return false;
		}

		auto cnk = itr->second;

		x -= ix * 16;
		y -= iy * 16;

		if (x < 0) {
			x = 16 + x;
		}

		if (y < 0) {
			y = 16 + y;
		}

		z = cnk->getHeight(x, y);
		return true;
	}

	void ChunkManager::updateLights() {
		for (auto& xi : mChunkMap) {
			for (auto& yi : xi.second) {
				yi.second->asyncUpdateLight();
			}
		}
	}
}