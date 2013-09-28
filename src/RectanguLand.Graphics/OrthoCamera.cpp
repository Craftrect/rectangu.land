#include "Precompiled.h"
#include "OrthoCamera.h"

using Utils::Matrix;
using Utils::Vector3;

namespace Graphics
{
	OrthoCamera::OrthoCamera(float width, float height, float zNear, float zFar) {
		mProjection = Matrix::ortho(0, width, 0, height, zNear, zFar);

		mWidth = width;
		mHeight = height;
		mNear = zNear;
		mFar = zFar;

		projChanged();
	}

	void OrthoCamera::setDimension(float width, float height) {
		bool hasChanged = false;
		if (width > 0) {
			hasChanged = true;
			mHeight = height;
		} else {
			width = mWidth;
		}

		if (height > 0) {
			hasChanged = true;
			mWidth = width;
		} else {
			height = mHeight;
		}

		if (hasChanged) {
			mProjection = Matrix::ortho(0, width, 0, height, mNear, mFar);
			projChanged();
		}
	}
}