#include "stdafx.h"
#include "KeyboardState.h"

namespace IO
{
	KeyboardState::KeyboardState() {
		for(uint32 i = 0; i < (1 << sizeof(uint16) * 8); ++i) {
			mStateArray[i] = false;
		}
	}

	bool KeyboardState::isKeyPressed(wchar_t key, bool strict) const {
		uint16 scan = VkKeyScan(key);
		uint8 vkey = scan & 0xFF;
		uint8 modifiers = (scan >> 8) & 0xFF;

		if(getKeyState(vkey) == false) {
			return false;
		}

		if((modifiers & 1) != 0) {
			if(getKeyState(VK_SHIFT) == false) {
				return false;
			}
		} else {
			if(strict && getKeyState(VK_SHIFT) == true) {
				return false;
			}
		}

		if((modifiers & 2) != 0) {
			if(getKeyState(VK_CONTROL) == false) {
				return false;
			}
		} else {
			if(strict && getKeyState(VK_CONTROL) == true) {
				return false;
			}
		}

		if((modifiers & 4) != 0) {
			if(getKeyState(VK_MENU) == false) {
				return false;
			}
		} else {
			if(strict && getKeyState(VK_MENU) == true) {
				return false;
			}
		}

		return true;
	}
};