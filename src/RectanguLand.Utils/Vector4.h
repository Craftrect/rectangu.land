#pragma once

#include "Vector3.h"

namespace Utils
{
	class Vector4
	{
		DirectX::XMFLOAT4 mVector;

	public:
		Vector4();
		Vector4(const Vector3& v3, float w);
		Vector4(float x, float y, float z, float w);


	};
}