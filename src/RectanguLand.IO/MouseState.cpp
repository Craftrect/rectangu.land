#include "stdafx.h"
#include "MouseState.h"

namespace IO
{
	MouseState::MouseState() {
		for(uint32 i = 0; i < 256; ++i) {
			mMouseButtons[i] = false;
		}
	}

	void MouseState::setButtonState(uint8 button, bool state) {
		mMouseButtons[button] = state;
	}

	bool MouseState::getButtonState(uint8 button) const {
		return mMouseButtons[button];
	}
}