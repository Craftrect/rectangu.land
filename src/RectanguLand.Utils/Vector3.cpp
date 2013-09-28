#include "PublicInclude.h"
#include "Vector3.h"

using namespace DirectX;

namespace Utils
{
	const Vector3 Vector3::UnitX(1, 0, 0);
	const Vector3 Vector3::UnitY(0, 1, 0);
	const Vector3 Vector3::UnitZ(0, 0, 1);

	Vector3::Vector3(const XMVECTOR& v) {
		XMStoreFloat3(&mVector, v);
	}

	Vector3::Vector3(float x, float y, float z) : Vector3() {
		X = x;
		Y = y;
		Z = z;
	}

	Vector3::Vector3() {
		XMStoreFloat3(&mVector, XMVectorZero());
	}

	float Vector3::getX() const {
		return mVector.x;
	}

	float Vector3::getY() const {
		return mVector.y;
	}

	float Vector3::getZ() const {
		return mVector.z;
	}

	void Vector3::setX(float x) {
		mVector.x = x;
	}

	void Vector3::setY(float y) {
		mVector.y = y;
	}

	void Vector3::setZ(float z) {
		mVector.z = z;
	}

	float Vector3::length() const {
		return XMVectorGetX(XMVector3Length(XMLoadFloat3(&mVector)));
	}

	float Vector3::lengthSquared() const {
		return XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&mVector)));
	}

	void Vector3::normalize() {
		XMStoreFloat3(&mVector, XMVector3Normalize(XMLoadFloat3(&mVector)));
	}

	Vector3 Vector3::normalized() const {
		return Vector3(XMVector3Normalize(XMLoadFloat3(&mVector)));
	}

	float Vector3::dot(const Vector3& other) const {
		return XMVectorGetX(XMVector3Dot(XMLoadFloat3(&mVector), XMLoadFloat3(&other.mVector)));
	}

	Vector3 Vector3::cross(const Vector3& other) const {
		return Vector3(XMVector3Cross(XMLoadFloat3(&mVector), XMLoadFloat3(&other.mVector)));
	}

	Vector3 Vector3::operator + (const Vector3& other) const {
		return Vector3(XMVectorAdd(XMLoadFloat3(&mVector), XMLoadFloat3(&other.mVector)));
	}

	Vector3 Vector3::operator - (const Vector3& other) const {
		return Vector3(XMVectorSubtract(XMLoadFloat3(&mVector), XMLoadFloat3(&other.mVector)));
	}

	Vector3 Vector3::operator * (float other) const {
		Vector3 tmp(other, other, other);
		return Vector3(XMVectorMultiply(XMLoadFloat3(&mVector), XMLoadFloat3(&tmp.mVector)));
	}

	Vector3 Vector3::operator / (float other) const {
		Vector3 tmp(other, other, other);
		return Vector3(XMVectorDivide(XMLoadFloat3(&mVector), XMLoadFloat3(&tmp.mVector)));
	}

	Vector3& Vector3::operator += (const Vector3& other) {
		XMStoreFloat3(&mVector, XMVectorAdd(XMLoadFloat3(&mVector), XMLoadFloat3(&other.mVector)));
		return *this;
	}

	Vector3& Vector3::operator -= (const Vector3& other) {
		XMStoreFloat3(&mVector, XMVectorSubtract(XMLoadFloat3(&mVector), XMLoadFloat3(&other.mVector)));
		return *this;
	}

	Vector3& Vector3::operator *= (float other) {
		Vector3 tmp(other, other, other);
		XMStoreFloat3(&mVector, XMVectorMultiply(XMLoadFloat3(&mVector), XMLoadFloat3(&tmp.mVector)));
		return *this;
	}

	Vector3& Vector3::operator /= (float other) {
		Vector3 tmp(other, other, other);
		XMStoreFloat3(&mVector, XMVectorDivide(XMLoadFloat3(&mVector), XMLoadFloat3(&tmp.mVector)));
		return *this;
	}
}