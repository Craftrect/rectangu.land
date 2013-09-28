#pragma once

#include "Camera.h"

namespace Graphics
{
	class OrthoCamera : public Camera
	{
		float mWidth;
		float mHeight;
		float mNear;
		float mFar;

	public:
		OrthoCamera(float width, float height, float zNear = 1.0f, float zFar = 100.0f);

		//! Changes the view dimension of the camera
		/*!
			@param	width	New width of the camera, value <= 0 means do not change
			@param	height	New height of the camera, value <= 0 means do not change.
		*/
		void setDimension(float width = -1, float height = -1);

		//! Changes the near and/or far clip of the camera
		/*!
			@param	zNear	New near clip of the camera, has to be < than zFar, <= 0 means do not change
			@param	zFar	New far clip of the camera, has to be > than zNear, <= 0 means do not change
		*/
		void setClip(float zNear = -1, float zFar = -1);
	};

	SHARED_TYPE(OrthoCamera);
}