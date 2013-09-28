#pragma once

#include "SceneNode.h"

namespace Graphics
{
	class SceneManager
	{
	private:
		SceneNodePtr mSceneRoot;

	public:
		void setSceneRoot(SceneNodePtr root);

		SceneNodePtr getRoot() const { return mSceneRoot; }

		void render();
		void update();
	};

	SHARED_TYPE(SceneManager);
}