#pragma once

#include "Vector3.h"

namespace Utils
{
	class SimplexNoise
	{
	private:
		static int32 grad[12][3];
		static int32 p[512];
		static bool permExtended;

		int32 ffloor(float value) const { return (value > 0) ? (int32) value : (int32) (value - 1); }
		float fdot(int32 (&g)[3], float x, float y, float z) const;

	public:
		SimplexNoise();

		float noise(float x, float y, float z);
		float noise(const Vector3& pos);
	};
}