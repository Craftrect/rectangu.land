#include "PublicInclude.h"
#include "PerlinNoise.h"

namespace Utils
{
	float PerlinNoise::noise(int32 x, int32 y) {
		int32 n = x + y * 57;
		n = (n << 13) ^ n;
		return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7FFFFFFF) / 1073741824.0f);
	}

	float PerlinNoise::smoothNoise(float x, float y) {
		float corners = (noise((int32) (x - 1), (int32) (y - 1)) + noise((int32) (x + 1), (int32) (y - 1)) + noise((int32) (x - 1), (int32) (y + 1))
			+ noise((int32) (x + 1), (int32) (y + 1))) / 16.0f;

		float sides = (noise((int32) (x - 1), (int32) y) + noise((int32) (x + 1), (int32) y) + noise((int32) x, (int32) (y + 1))
			+ noise((int32) x, (int32) (y - 1))) / 8.0f;

		float center = noise((int32) x, (int32) y) / 4.0f;

		return corners + sides + center;
	}

	float PerlinNoise::interpolate(float a, float b, float t) {
		float ft = t * 3.1415926f;
		float f = (1 - cosf(ft)) * 0.5f;

		return a * (1 - f) + b * f;
	}

	float PerlinNoise::interpolatedNoise(float x, float y) {
		float ix = floor(x);
		float iy = floor(y);

		float remx = x - ix;
		float remy = y - iy;

		float v1 = smoothNoise(ix    , iy);
		float v2 = smoothNoise(ix + 1, iy);
		float v3 = smoothNoise(ix    , iy + 1);
		float v4 = smoothNoise(ix + 1, iy + 1);

		float i1 = interpolate(v1, v2, remx);
		float i2 = interpolate(v3, v4, remx);

		return interpolate(i1, i2, remy);
	}

	float PerlinNoise::sample(float x, float y) {
		float total = 0.0f;
		float p = mPersistence;
		
		float amplitude = 1;
		float maxValue = 0.0f;

		for (int32 i = 0; i < mOctaves; ++i) {
			int32 freq = 1 << i;

			total += interpolatedNoise(x * freq, y * freq) * amplitude;
			maxValue += amplitude;

			amplitude *= p;
		}

		return total / maxValue;
	}
}