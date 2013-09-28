#include "stdafx.h"
#include "WorldManager.h"
#include "MapChunk.h"
#include "ChunkManager.h"

INIT_SINGLETON(Terrain::WorldManager);

namespace Terrain
{
	void WorldManager::render() {
		sWorldFrame->captureSSAO();

		SceneNode::renderChildren();

		sWorldFrame->endCaptureSSAO();
	}

	void WorldManager::enterWorld(int32 x, int32 y) {
		for (uint32 i = 0; i < 5; ++i) {
			for (uint32 j = 0; j < 5; ++j) {
				MapChunkPtr chnk = std::make_shared<MapChunk>(x + j, y + i);
				chnk->asyncLoad();
				
				addChild(chnk);

				sChunkMgr->addChunk(chnk);
			}
		}

		sChunkMgr->updateLights();
	}

	void WorldManager::update() {

	}
}