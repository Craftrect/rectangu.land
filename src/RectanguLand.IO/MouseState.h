#pragma once

namespace IO
{
	enum MouseButton
	{
#ifdef _WIN32
		Left = VK_LBUTTON,
		Right = VK_RBUTTON,
		Middle = VK_MBUTTON,
		X1 = VK_XBUTTON1,
		X2 = VK_XBUTTON2
#endif
	};

	class MouseState
	{
		bool mMouseButtons[256];

	public:
		MouseState();

		void setButtonState(uint8 button, bool state);
		bool getButtonState(uint8 button) const;
		bool getButtonState(MouseButton button) const { return getButtonState((uint8)button); }
	};

	SHARED_TYPE(MouseState);
}