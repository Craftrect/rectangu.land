#pragma once

#include "Mouse.h"
#include "Keyboard.h"

namespace IO
{
	class IoManager : public Singleton<IoManager>
	{
	private:
		MousePtr mMouse;
		KeyboardPtr mKeyboard;

	public:
		IoManager();

		void init(Graphics::RenderTargetEntryPtr target);

		MousePtr getMouse() const { return mMouse; }
		KeyboardPtr getKeyboard() const { return mKeyboard; }
	};
}

#define sIoMgr (IO::IoManager::getInstance())