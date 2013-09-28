#include "stdafx.h"
#include "Keyboard.h"
#include "KeyboardState.h"

namespace IO
{
	Keyboard::Keyboard() {
		mState = std::make_shared<KeyboardState>();
	}

	void Keyboard::initKeyboard(Graphics::RenderTargetEntryPtr target) {
		auto any_handle = target->getProperty(Graphics::RenderTargetEntry::Properties::WindowHandle);
		if(any_handle.empty() == true) {
			throw std::invalid_argument("Rendertargetentry does not have a window handle.");
		}

		HWND hWindow = boost::any_cast<HWND>(any_handle);

		SetProp(hWindow, L"PROP_KEYBOARD_LISTENER", (HANDLE)this);
		mOldProc = (WNDPROC)SetWindowLongPtr(hWindow, GWLP_WNDPROC, (LONG)&Keyboard::SubclassProc);
	}

	LRESULT CALLBACK Keyboard::SubclassProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		Keyboard* keyboard = reinterpret_cast<Keyboard*>(GetProp(hWindow, L"PROP_KEYBOARD_LISTENER"));
		if(keyboard == nullptr) {
			return DefWindowProc(hWindow, uMsg, wParam, lParam);
		}

		return keyboard->onMessage(hWindow, uMsg, wParam, lParam);
	}

	LRESULT Keyboard::onMessage(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch(uMsg) {
		case WM_KEYDOWN:
			mState->setKeyState((uint16)wParam, true);
			break;

		case WM_KEYUP:
			mState->setKeyState((uint16)wParam, false);
			break;

		case WM_SYSKEYUP:
			mState->setKeyState((uint16)wParam, true);
			break;

		case WM_SYSKEYDOWN:
			mState->setKeyState((uint16)wParam, false);
			break;

		case WM_DEADCHAR:
			break;
		}

		if(mOldProc == nullptr) {
			return DefWindowProc(hWindow, uMsg, wParam, lParam);
		} else {
			return CallWindowProc(mOldProc, hWindow, uMsg, wParam, lParam);
		}
	}
};