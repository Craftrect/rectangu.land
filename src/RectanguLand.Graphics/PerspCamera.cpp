#include "Precompiled.h"
#include "PerspCamera.h"

using Utils::Matrix;

namespace Graphics
{
	PerspCamera::PerspCamera(float fovy, float aspect, float zNear, float zFar) {
		zFar = 400;
		mProjection = Matrix::perspective(fovy, aspect, zNear, zFar);

		mNear   = zNear;
		mFar    = zFar;
		mFov    = fovy;
		mAspect = aspect;

		projChanged();
	}

	PerspCamera::PerspCamera(float fovy, uint32 w, uint32 h, float zNear, float zFar) :
		PerspCamera(fovy, (float) w / (float) h, zNear, zFar) {

	}

	void PerspCamera::onResize(uint32 width, uint32 height) {
		float aspect = (float) width / (float) height;

		mAspect = aspect;

		mProjection = Matrix::perspective(mFov, aspect, mNear, mFar);

		projChanged();
	}
}