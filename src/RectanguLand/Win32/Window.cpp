#include "Precompiled.h"
#include "Window.h"

namespace Win32
{
	Window::Window() : mClassName(_T("GxWindowClass")), mIsQuit(false) {
		memset(&mWndClass, 0, sizeof(WNDCLASSEX));

		mWndClass.cbSize = sizeof(WNDCLASSEX);
		mWndClass.hbrBackground = GetSysColorBrush(GetSysColor(COLOR_WINDOW));
		mWndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		mWndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		mWndClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
		mWndClass.hInstance = GetModuleHandle(nullptr);
		mWndClass.lpfnWndProc = WndProc;
		mWndClass.lpszClassName = mClassName.c_str();
		mWndClass.style = CS_HREDRAW | CS_VREDRAW;

		RegisterClassEx(&mWndClass);

		mWindow = CreateWindow(mClassName.c_str(), _T("RectanguLand"), WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, GetModuleHandle(nullptr), this);

		mProperties[Graphics::RenderTargetEntry::Properties::WindowHandle] = boost::any(mWindow);

		RECT rcWindow;
		GetClientRect(mWindow, &rcWindow);

		mProperties[Graphics::RenderTargetEntry::Properties::Width] = boost::any((float) (rcWindow.right - rcWindow.left));
		mProperties[Graphics::RenderTargetEntry::Properties::Height] = boost::any((float) (rcWindow.bottom - rcWindow.top));
	}

	LRESULT CALLBACK Window::WndProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		Window* wnd = static_cast<Window*>(GetProp(hWindow, _T("PROP_WND_INSTANCE")));

		if (wnd != nullptr) {
			return wnd->onMessage(uMsg, wParam, lParam);
		}

		switch (uMsg) {
		case WM_CREATE:
			{
				LPCREATESTRUCT createStruct = (LPCREATESTRUCT) lParam;

				SetProp(hWindow, _T("PROP_WND_INSTANCE"), (HANDLE) createStruct->lpCreateParams);
			}
			break;
		}

		return DefWindowProc(hWindow, uMsg, wParam, lParam);
	}

	void Window::parseMessages() {
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	LRESULT Window::onMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
		case WM_CLOSE:
			{
				PostQuitMessage(0);
			}
			break;

		case WM_DESTROY:
			{
				mIsQuit = true;
			}
			break;
		}

		return DefWindowProc(mWindow, uMsg, wParam, lParam);
	}

	boost::any Window::getProperty(Graphics::RenderTargetEntry::Properties prop) const {
		auto itr = mProperties.find(prop);
		if (itr == mProperties.end()) {
			return boost::any();
		}

		return itr->second;
	}

	void Window::setProperty(Graphics::RenderTargetEntry::Properties prop, const boost::any& value) {
		auto itr = mProperties.find(prop);
		if (itr == mProperties.end()) {
			throw std::invalid_argument("Property type is not recognized.");
		}

		mProperties[prop] = value;
	}

	void Window::setVisible(bool visible) {
		if (visible) {
			ShowWindow(mWindow, SW_SHOW);
		} else {
			ShowWindow(mWindow, SW_HIDE);
		}
	}
};