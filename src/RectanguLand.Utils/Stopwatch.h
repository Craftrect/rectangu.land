#pragma once

namespace Utils
{
	class Stopwatch
	{
		std::chrono::system_clock::time_point mStart;
		std::chrono::system_clock::time_point mEnd;

	public:
		void start();
		void stop();
		uint64 getMilliseconds() const;
	};
}