#include "PublicInclude.h"
#include "MersenneTwister.h"

INIT_SINGLETON(Utils::MersenneTwister);

namespace Utils
{
	MersenneTwister::MersenneTwister() {
		A[0] = 0;
		A[1] = 0x9908B0DF;

		for (i = 1; i < N; ++i) {
			y[i] = (1812433253 * (y[i - 1] ^ (y[i - 1] >> 30)) + i);
		}
	}

	uint32 MersenneTwister::nextUInt32() {
		if (i >= N) {
			uint32 h = 0;
			for (i = 0; i < N - M; ++i) {
				h = (y[i] & HI) | (y[i + 1] & LO);
				y[i] = y[i + M] ^ (h >> 1) ^ A[h & 1];
			}

			for ( ; i < N - 1; ++i) {
				h = (y[i] & HI) | (y[i + 1] & LO);
				y[i] = y[i + (M - N)] ^ (h >> 1) ^ A[h & 1];
			}

			h = (y[N - 1] & HI) | (y[0] & LO);
			y[N - 1] = y[M - 1] ^ (h >> 1) ^ A[h & 1];
			i = 0;
		}

		uint32 e = y[i++];
		
		e ^= (e >> 11);
		e ^= (e << 7) & 0x9D2C5680;
		e ^= (e << 15) & 0xEFC60000;
		e ^= (e >> 18);

		return e;
	}

	double MersenneTwister::nextDouble() {
		return (double) nextUInt32() / (double) std::numeric_limits<uint32>::max();
	}

	uint32 MersenneTwister::next(uint32 minValue, uint32 maxValue) {
		if (maxValue <= minValue) {
			return minValue;
		}

		uint32 diff = maxValue - minValue;
		return (uint32) (minValue + diff * nextDouble());
	}

	double MersenneTwister::nextDouble(double minVal, double maxVal) {
		if (maxVal <= minVal) {
			return minVal;
		}

		double diff = maxVal - minVal;
		return minVal + nextDouble() * diff;
	}
}