#pragma once

#include "Camera.h"

namespace Graphics
{
	//+ Perspectivic Camera
	class PerspCamera : public Camera
	{
	private:
		float mAspect;
		float mFov;
		float mNear;
		float mFar;

	public:
		PerspCamera(float fovy, float aspect, float zNear = 1, float zFar = 100);
		PerspCamera(float fovy, uint32 width, uint32 height, float zNear = 1, float zFar = 200);

		void onResize(uint32 width, uint32 height);
	};

	SHARED_TYPE(PerspCamera);
}