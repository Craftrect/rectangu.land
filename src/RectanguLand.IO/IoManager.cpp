#include "stdafx.h"
#include "IoManager.h"

INIT_SINGLETON(IO::IoManager);

namespace IO
{
	IoManager::IoManager() {
		mMouse = std::make_shared<Mouse>();
		mKeyboard = std::make_shared<Keyboard>();
	}

	void IoManager::init(Graphics::RenderTargetEntryPtr target) {
		mMouse->init(target);
		mKeyboard->initKeyboard(target);
	}
};