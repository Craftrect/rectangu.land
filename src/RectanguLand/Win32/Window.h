#pragma once

namespace Win32
{
	class Window : public Graphics::RenderTargetEntry
	{
		std::map<Properties, boost::any> mProperties;

		HWND mWindow;
		WNDCLASSEX mWndClass;
		std::tstring mClassName;
		bool mIsQuit;

		static LRESULT CALLBACK WndProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);
		
		LRESULT onMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	public:
		Window();

		HWND getHandle() const { return mWindow; }

		void parseMessages();
		void setVisible(bool visible);

		boost::any getProperty(Properties prop) const;
		void setProperty(Properties prop, const boost::any& value);

		bool isQuit() const { return mIsQuit; }
	};
};