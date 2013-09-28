#pragma once

#ifdef _WIN32

#include <DirectXMath.h>

#endif

namespace Utils
{
	class Vector3
	{
	private:
		DirectX::XMFLOAT3 mVector;

	public:
		Vector3();
		Vector3(const DirectX::XMVECTOR& vec);
		Vector3(float x, float y, float z);

		float length() const;
		float lengthSquared() const;
		float dot(const Vector3& other) const;
		Vector3 cross(const Vector3& other) const;
		Vector3 normalized() const;
		void normalize();

		float getX() const;
		float getY() const;
		float getZ() const;

		void setX(float x);
		void setY(float y);
		void setZ(float z);

		Vector3 operator + (const Vector3& other) const;
		Vector3 operator - (const Vector3& other) const;
		Vector3 operator * (float val) const;
		Vector3 operator / (float val) const;

		Vector3& operator += (const Vector3& other);
		Vector3& operator -= (const Vector3& other);
		Vector3& operator *= (float val);
		Vector3& operator /= (float val);

		operator DirectX::XMFLOAT3* () {
			return &mVector;
		}

		operator const DirectX::XMFLOAT3* () const {
			return &mVector;
		}

		__declspec(property(get = getX, put = setX)) float X;
		__declspec(property(get = getY, put = setY)) float Y;
		__declspec(property(get = getZ, put = setZ)) float Z;

		static Vector3 cross(const Vector3& v1, const Vector3& v2) { return v1.cross(v2); }

		static const Vector3 UnitX;
		static const Vector3 UnitY;
		static const Vector3 UnitZ;
	};
}