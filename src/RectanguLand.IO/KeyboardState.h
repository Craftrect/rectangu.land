#pragma once

namespace IO
{
	SHARED_FORWARD(KeyboardState);

	enum class Modifiers
	{
		Control,
		Shift,
		Alt,
		Help,
		Command,
		Function,
		Numpad
	};

	class KeyEntry
	{
		uint16 mCode;
		uint32 mModifiers;
		bool mIsDown;

		template<typename M, typename... T>
		uint32 combineModifiers(uint32& cur, const M& modifier, const T&... remain) const {
			static_assert(std::is_same<M, Modifiers>::value, "T must be of type Modifier!");

			cur |= (uint32)modifier;
			combineModifiers(cur, remain...);
			return cur;
		}

		template<typename M>
		uint32 combineModifiers(uint32& cur, const M& modifier) const {
			static_assert(std::is_same<M, Modifiers>::value, "T must be of type Modifier!");

			cur |= (uint32)modifier;
			return cur;
		}

	public:
		KeyEntry() : mCode(0), mModifiers(0), mIsDown(false) {
		}

		KeyEntry(uint16 code) {
			mIsDown = false;
			mCode = code;
			mModifiers = 0;
		}

		template<typename... T>
		KeyEntry(uint16 code, const T&... modifiers) {
			mIsDown = false;
			mModifiers = 0;
			combineModifiers(mModifiers, modifiers...);
		}

		template<typename... T>
		bool hasModifiers(const T&... modifires) const {
			uint32 mods = 0;
			combineModifiers(mods, modifiers...);
			return (mModifiers & mods) == mods;
		}

		void setState(bool pressed) {
			mIsDown = pressed;
		}

		bool getState() const {
			return mIsDown;
		}
	};

	class KeyboardState
	{
	protected:
		bool mStateArray[1 << sizeof(uint16) * 8];

	public:
		KeyboardState();

		void setKeyState(uint16 virtualKey, bool pressed) { mStateArray[virtualKey] = pressed; }
		bool getKeyState(uint16 virtualKey) const { return mStateArray[virtualKey]; }

		//! Checks if all the keys for a character are pressed (e.g. including SHIFT, CTRL, ...)
		//! \param	key		unicode character representation
		//! \param	strict	true if pressing a modifier that is not expected should return false (like CTRL + 'w' for 'w' would fail)
		//! \return	boolean indicating if all the keys neccessary are pressed
		bool isKeyPressed(wchar_t key, bool strict = true) const;
	};
}