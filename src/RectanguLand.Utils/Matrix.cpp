#include "PublicInclude.h"
#include "Matrix.h"

using namespace DirectX;

namespace Utils
{
	Matrix::Matrix() {
		XMStoreFloat4x4(&mMatrix, XMMatrixIdentity());
	}

	Matrix::Matrix(const XMMATRIX& mat) {
		XMStoreFloat4x4(&mMatrix, mat);
	}

	bool Matrix::isInfinite() const {
		return XMMatrixIsInfinite(XMLoadFloat4x4(&mMatrix));
	}
	
	float Matrix::determinant() const {
		return XMVectorGetX(XMMatrixDeterminant(XMLoadFloat4x4(&mMatrix)));
	}

	bool Matrix::invert() {
		XMVECTOR detv;
		auto matTmp = XMMatrixInverse(&detv, XMLoadFloat4x4(&mMatrix));
		if (XMMatrixIsInfinite(matTmp)) {
			return false;
		}

		XMStoreFloat4x4(&mMatrix, matTmp);
		return true;
	}

	Matrix Matrix::inverted() const {
		XMVECTOR detv;
		return Matrix(XMMatrixInverse(&detv, XMLoadFloat4x4(&mMatrix)));
	}

	void Matrix::transpose() {
		XMStoreFloat4x4(&mMatrix, XMMatrixTranspose(XMLoadFloat4x4(&mMatrix)));
	}

	void Matrix::transposed(Matrix& out) const {
		XMStoreFloat4x4(&out.mMatrix, XMMatrixTranspose(XMLoadFloat4x4(&mMatrix)));
	}

	Vector3 Matrix::operator * (const Vector3& v) const {
		return Vector3(XMVector3TransformCoord(XMLoadFloat3(v), XMLoadFloat4x4(&mMatrix)));
	}

	Matrix Matrix::operator * (const Matrix& m) const {
		return Matrix(XMMatrixMultiply(XMLoadFloat4x4(&mMatrix), XMLoadFloat4x4(&m.mMatrix)));
	}

	Matrix& Matrix::operator *= (const Matrix& m) {
		XMStoreFloat4x4(&mMatrix, XMMatrixMultiply(XMLoadFloat4x4(&mMatrix), XMLoadFloat4x4(&m.mMatrix)));

		return *this;
	}

	Matrix Matrix::scaling(float x, float y, float z) {
		return Matrix(XMMatrixScaling(x, y, z));
	}

	Matrix Matrix::scaling(const Vector3& v) {
		return Matrix(XMMatrixScalingFromVector(XMLoadFloat3(v)));
	}

	Matrix Matrix::translation(float x, float y, float z) {
		return Matrix(XMMatrixTranslation(x, y, z));
	}

	Matrix Matrix::translation(const Vector3& v) {
		return Matrix(XMMatrixTranslationFromVector(XMLoadFloat3(v)));
	}

	Matrix Matrix::rotation(float x, float y, float z) {
		return Matrix(XMMatrixRotationRollPitchYaw(x, y, z));
	}

	Matrix Matrix::rotation(const Vector3& v) {
		return Matrix(XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(v)));
	}

	Matrix Matrix::rotationAxis(const Vector3& v, float amount) {
		return Matrix(XMMatrixRotationAxis(XMLoadFloat3(v), XMConvertToRadians(amount)));
	}

	Matrix Matrix::lookAt(const Vector3& eye, const Vector3& target, const Vector3& up) {
		return Matrix(XMMatrixLookAtLH(XMLoadFloat3(eye), XMLoadFloat3(target), XMLoadFloat3(up)));
	}

	Matrix Matrix::perspective(float fovy, float aspect, float zNear, float zFar) {
		return Matrix(XMMatrixPerspectiveFovLH(XMConvertToRadians(fovy), aspect, zNear, zFar));
	}

	Matrix Matrix::ortho(float left, float right, float up, float bottom, float zNear, float zFar) {
		return Matrix(XMMatrixOrthographicOffCenterLH(left, right, bottom, up, zNear, zFar));
	}
}