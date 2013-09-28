#include "Precompiled.h"
#include "Camera.h"

using Utils::Vector3;
using Utils::Matrix;

namespace Graphics
{
	Camera::Camera() {
		mPosition = Vector3(10, 0, 0);
		mTarget = Vector3(9, 0, 0);
		mUp = Vector3(0, 0, 1);
		mRight = Vector3(0, -1, 0);

		updateView();
	}

	void Camera::updateView() {
		mView = Matrix::lookAt(mPosition, mTarget, mUp);
		mInvView = mView.inverted();

		ViewChanged(mView);
	}

	void Camera::setPosition(const Vector3& pos) {
		auto forward = (mTarget - mPosition).normalized();
		mPosition = pos;
		mTarget = mPosition + forward;

		updateView();
	}

	void Camera::setTarget(const Vector3& target) {
		mTarget = target;

		updateView();
	}

	void Camera::move(const Vector3& direction, float amount) {
		auto toMove = direction * amount;

		mPosition += toMove;
		mTarget += toMove;

		updateView();
	}

	void Camera::yaw(float amount) {
		Matrix mat = Matrix::rotationAxis(Vector3::UnitZ, amount);
		auto forward = (mTarget - mPosition).normalized();

		forward = mat * forward;
		forward.normalize();

		mTarget = mPosition + forward;

		mUp = mat * mUp;
		mUp.normalize();

		mRight = mat * mRight;
		mRight.normalize();

		updateView();
	}

	void Camera::pitch(float amount) {
		Matrix mat = Matrix::rotationAxis(mRight, amount);

		auto forward = (mTarget - mPosition).normalized();

		forward = mat * forward;
		forward.normalize();

		mTarget = mPosition + forward;

		mUp = mat * mUp;
		mUp.normalize();

		updateView();
	}

	void Camera::roll(float amount) {
		auto forward = (mTarget - mPosition).normalized();

		Matrix mat = Matrix::rotationAxis(forward, amount);

		mUp = mat * mUp;
		mUp.normalize();

		mRight = mat * mRight;
		mRight.normalize();

		updateView();
	}

	void Camera::projChanged() {
		mInvProj = mProjection.inverted();

		ProjChanged(mProjection);
	}
}