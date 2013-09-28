#pragma once

#include "KeyboardState.h"

namespace IO
{
	SHARED_FORWARD(Keyboard);
	SHARED_FORWARD(KeyboardState);

	class Keyboard
	{
		KeyboardStatePtr mState;
#ifdef _WIN32
	protected:
		WNDPROC mOldProc;

		LRESULT onMessage(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);

		static LRESULT CALLBACK SubclassProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);
	public:
		Keyboard();

		void initKeyboard(Graphics::RenderTargetEntryPtr target);
#endif

		KeyboardStatePtr getState() const { return mState; }
	};
};