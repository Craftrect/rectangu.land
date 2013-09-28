#pragma once

#include "Singleton.h"

namespace Utils
{
	class MersenneTwister : public Singleton<MersenneTwister>
	{
		static const unsigned int N = 624;
		static const unsigned int M = 397;
		static const unsigned int HI = 0x80000000;
		static const unsigned int LO = 0x7FFFFFFF;

		uint32 mSeed;
		uint32 A[2];
		uint32 y[N];
		uint32 i;

	public:
		MersenneTwister();

		uint32 nextUInt32();
		uint32 next(uint32 min, uint32 max);
		double nextDouble();
		double nextDouble(double min, double max);
	};
}

#define sRandom (Utils::MersenneTwister::getInstance())