#pragma once

#include "SceneNode.h"

namespace Graphics
{
	class SceneRoot : public SceneNode
	{
	public:
		SceneRoot();
		virtual void render();
		virtual void update();
	};

	SHARED_TYPE(SceneRoot);
}