#include "PublicInclude.h"
#include "Vector4.h"

using namespace DirectX;

namespace Utils
{
	Vector4::Vector4() : Vector4(0, 0, 0, 0) {

	}

	Vector4::Vector4(const Vector3& v3, float w) : Vector4(v3.X, v3.Y, v3.Z, w) {

	}

	Vector4::Vector4(float x, float y, float z, float w) {
		XMStoreFloat4(&mVector, XMVectorSet(x, y, z, w));
	}
}