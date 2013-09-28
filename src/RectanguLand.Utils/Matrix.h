#pragma once

#ifdef _WIN32

#include <DirectXMath.h>

#endif

#include "Vector3.h"

namespace Utils
{
	class Matrix
	{
		DirectX::XMFLOAT4X4 mMatrix;

	public:
		Matrix();
		Matrix(const DirectX::XMMATRIX& mat);

		void transpose();
		void transposed(Matrix& out) const;
		bool invert();
		Matrix inverted() const;
		float determinant() const;
		bool isInfinite() const;

		Vector3 operator * (const Vector3& v) const;
		Matrix operator * (const Matrix& m) const;

		Matrix& operator *= (const Matrix& m);

		static Matrix scaling(float x, float y, float z);
		static Matrix scaling(const Vector3& v);
		static Matrix translation(float x, float y, float z);
		static Matrix translation(const Vector3& v);
		static Matrix rotation(float x, float y, float z);
		static Matrix rotation(const Vector3& v);
		static Matrix rotationAxis(const Vector3& axis, float amount);
		static Matrix lookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
		static Matrix perspective(float fovy, float aspect, float zNear, float zFar);
		static Matrix ortho(float left, float right, float up, float bottom, float zNear, float zFar);
	};
}