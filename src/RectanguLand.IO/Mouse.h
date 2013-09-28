#pragma once

#include "MouseState.h"

namespace IO
{
	class Mouse
	{
#ifdef _WIN32
		WNDPROC mOldProc;

		static LRESULT WINAPI WndProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);

		LRESULT onMessage(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

		MouseStatePtr mState;

	public:
		void init(Graphics::RenderTargetEntryPtr target);

		MouseStatePtr getState() const { return mState; }
	};

	SHARED_TYPE(Mouse);
}