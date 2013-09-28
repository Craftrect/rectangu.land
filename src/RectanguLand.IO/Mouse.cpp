#include "stdafx.h"
#include "Mouse.h"

namespace IO
{
	void Mouse::init(Graphics::RenderTargetEntryPtr target) {
#ifdef _WIN32
		auto any_handle = target->getProperty(Graphics::RenderTargetEntry::Properties::WindowHandle);
		if(any_handle.empty() == true) {
			throw std::invalid_argument("Rendertargetentry does not have a window handle.");
		}

		HWND hWnd = boost::any_cast<HWND>(any_handle);

		SetProp(hWnd, L"PROP_MOUSE_LISTENER", (HANDLE)this);
		mOldProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)WndProc);
#endif
	}

	LRESULT WINAPI Mouse::WndProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		Mouse* mouse = reinterpret_cast<Mouse*>(GetProp(hWindow, L"PROP_MOUSE_LISTENER"));
		if(mouse == nullptr) {
			return DefWindowProc(hWindow, uMsg, wParam, lParam);
		} else {
			return mouse->onMessage(hWindow, uMsg, wParam, lParam);
		}
	}

	LRESULT Mouse::onMessage(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch(uMsg) {
		case WM_LBUTTONDOWN:
			mState->setButtonState(VK_LBUTTON, true);
			break;

		case WM_LBUTTONUP:
			mState->setButtonState(VK_LBUTTON, false);
			break;

		case WM_RBUTTONDOWN:
			mState->setButtonState(VK_RBUTTON, true);
			break;

		case WM_RBUTTONUP:
			mState->setButtonState(VK_RBUTTON, false);
			break;

		case WM_MBUTTONDOWN:
			mState->setButtonState(VK_MBUTTON, true);
			break;

		case WM_MBUTTONUP:
			mState->setButtonState(VK_MBUTTON, false);
			break;

		case WM_XBUTTONDOWN:
			mState->setButtonState(HIWORD(wParam) == 1 ? VK_XBUTTON1 : VK_XBUTTON2, true);
			break;

		case WM_XBUTTONUP:
			mState->setButtonState(HIWORD(wParam) == 1 ? VK_XBUTTON1 : VK_XBUTTON2, false);
			break;
		}

		if(mOldProc != nullptr) {
			return CallWindowProc(mOldProc, hWindow, uMsg, wParam, lParam);
		} else {
			return DefWindowProc(hWindow, uMsg, wParam, lParam);
		}
	}
}