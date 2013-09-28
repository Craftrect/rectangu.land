#include "Precompiled.h"
#include "SceneNode.h"

namespace Graphics
{
	SceneNode::SceneNode() : mNodeName("Scene.UnnamedNode") {

	}

	SceneNode::SceneNode(const String& name) : mNodeName(name) {

	}

	void SceneNode::addChild(SceneNodePtr child) {
		if (child->isTransparent() == false) {
			mOpaqueChildren.push_back(child);
		} else {
			if (mTransparentChildren.size() == 0) {
				mTransparentChildren.push_back(child);
			} else {
				auto itr = mTransparentChildren.begin();
				while (itr != mTransparentChildren.end()) {
					if ((*itr)->getDepth() <= child->getDepth()) {
						break;
					}
				}

				mTransparentChildren.insert(itr, child);
			}
		}
	}

	void SceneNode::renderChildren() {
		for (auto& child : mOpaqueChildren) {
			child->render();
		}

		for (auto& child : mTransparentChildren) {
			child->render();
		}
	}

	void SceneNode::updateChildren() {
		for (auto& child : mOpaqueChildren) {
			child->update();
		}

		for (auto& child : mTransparentChildren) {
			child->update();
		}
	}
}