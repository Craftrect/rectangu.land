#include "PublicInclude.h"
#include "Stopwatch.h"

namespace Utils
{
	void Stopwatch::start() {
		mStart = std::chrono::steady_clock::now();
	}

	void Stopwatch::stop() {
		mEnd = std::chrono::steady_clock::now();
	}

	uint64 Stopwatch::getMilliseconds() const {
		return std::chrono::duration_cast<std::chrono::milliseconds>(mEnd - mStart).count();
	}
}