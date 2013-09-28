#include "Precompiled.h"
#include "SceneRoot.h"

namespace Graphics
{
	SceneRoot::SceneRoot() : SceneNode("Root") {

	}

	void SceneRoot::update() {
		SceneNode::updateChildren();
	}

	void SceneRoot::render() {
		SceneNode::renderChildren();
	}
}
