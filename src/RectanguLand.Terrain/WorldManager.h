#pragma once

namespace Terrain
{
	class WorldManager : public Singleton<WorldManager>, public Graphics::SceneNode
	{
	public:
		void enterWorld(int32 x, int32 y);
		void render();
		void update();
	};
}

#define sWorldManager (Terrain::WorldManager::getInstance())