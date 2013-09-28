#include "Precompiled.h"
#include "SceneManager.h"

namespace Graphics
{
	void SceneManager::render() {
		if (mSceneRoot != nullptr) {
			mSceneRoot->render();
		}
	}

	void SceneManager::update() {
		if (mSceneRoot != nullptr) {
			mSceneRoot->update();
		}
	}

	void SceneManager::setSceneRoot(SceneNodePtr root) {
		mSceneRoot = root;
	}
}