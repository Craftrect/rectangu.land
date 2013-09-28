#pragma once

namespace Utils
{
	class PerlinNoise
	{
	private:
		static float noise(int32 x, int32 y);
		static float smoothNoise(float x, float y);
		static float interpolatedNoise(float x, float y);
		static float interpolate(float a, float b, float t);

		float mPersistence;
		int32 mOctaves;

	public:
		float sample(float x, float y);

		void setPersistence(float pers) { mPersistence = pers; }
		void setOctaves(int32 octaves) { mOctaves = octaves; }
	};
}