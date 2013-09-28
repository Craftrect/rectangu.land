#pragma once

#include "Singleton.h"

enum class LogLevel
{
	Error,
	Notice,
	Debug,
};

class Log : public Singleton<Log>
{
#ifdef _WIN32
	HANDLE mCmd;
#endif

	LogLevel mLevel;
	std::mutex mLock;

	template<typename T, typename... Args>
	void expandLogMessage(std::wstringstream& strm, const T& arg, const Args&... remain) {
		strm << arg;
		expandLogMessage<Args...>(strm, remain...);
	}

	template<typename T>
	void expandLogMessage(std::wstringstream& strm, const T& arg) {
		strm << arg;
	}

	void printTimestamp(std::wstringstream& strm);
public:
	Log();

	void setLogLevel(LogLevel level){ std::lock_guard<std::mutex> l(mLock); mLevel = level; }

	template<typename... Args>
	void debug(const String& title, const Args&... message) {
		std::lock_guard<std::mutex> l(mLock);

		if (mLevel < LogLevel::Debug) {
			return;
		}

		SetConsoleTextAttribute(mCmd, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::wstringstream strm;

		printTimestamp(strm);

		strm << L" D " << (std::wstring)title << L": ";

		DWORD numWritten = 0;
		WriteConsole(mCmd, strm.str().c_str(), strm.str().length(), &numWritten, nullptr);

		SetConsoleTextAttribute(mCmd, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

		strm = std::wstringstream();
		expandLogMessage<Args...>(strm, message...);
		strm << std::endl;

		WriteConsole(mCmd, strm.str().c_str(), strm.str().length(), &numWritten, nullptr);
	}

	template<typename... Args>
	void notice(const String& title, const Args&... message) {
		std::lock_guard<std::mutex> l(mLock);

		if (mLevel < LogLevel::Notice) {
			return;
		}

		SetConsoleTextAttribute(mCmd, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		std::wstringstream strm;

		printTimestamp(strm);

		strm << L" N " << (std::wstring)title << L": ";

		DWORD numWritten = 0;
		WriteConsole(mCmd, strm.str().c_str(), strm.str().length(), &numWritten, nullptr);

		SetConsoleTextAttribute(mCmd, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

		strm = std::wstringstream();
		expandLogMessage<Args...>(strm, message...);
		strm << std::endl;

		WriteConsole(mCmd, strm.str().c_str(), strm.str().length(), &numWritten, nullptr);
	}
	
	template<typename... Args>
	void error(const String& title, const Args&... message) {
		std::lock_guard<std::mutex> l(mLock);

		if (mLevel < LogLevel::Error) {
			return;
		}

		SetConsoleTextAttribute(mCmd, FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::wstringstream strm;

		printTimestamp(strm);

		strm << L" E " << (std::wstring)title << L": ";

		DWORD numWritten = 0;
		WriteConsole(mCmd, strm.str().c_str(), strm.str().length(), &numWritten, nullptr);

		SetConsoleTextAttribute(mCmd, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

		strm = std::wstringstream();
		expandLogMessage<Args...>(strm, message...);
		strm << std::endl;

		WriteConsole(mCmd, strm.str().c_str(), strm.str().length(), &numWritten, nullptr);
	}
};

#define sLog (Log::getInstance())