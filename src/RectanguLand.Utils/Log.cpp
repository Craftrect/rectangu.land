#include "PublicInclude.h"
#include "Log.h"

INIT_SINGLETON(Log);

Log::Log() {
#ifdef _DEBUG
	setLogLevel(LogLevel::Debug);
#else
	setLogLevel(LogLevel::Notice);
#endif

#ifdef _WIN32
#ifdef _DEBUG
	AllocConsole();

	mCmd = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
#endif
}

void Log::printTimestamp(std::wstringstream& strm) {
	auto curtime = time(nullptr);
	tm time;
	localtime_s(&time, &curtime);

	strm << std::setw(2) << std::setfill(L'0') << time.tm_hour << L":" << std::setw(2) << std::setfill(L'0') << time.tm_min;
}