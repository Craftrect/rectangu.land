#pragma once

#include "Objects/Buffer.h"
#include "ShaderCode.h"
#include "GxDevice.h"
#include "Camera.h"
#include "Effect.h"

namespace Graphics
{
	class GlobalBufferManager
	{
		struct MatrixBuffer
		{
			Utils::Matrix World, View, Projection;
		};

		MatrixBuffer mMatrixBuffer;

		std::map<ShaderVariables, BufferPtr> mBuffers;

		void cameraMatrixChanged(CameraPtr camera, const Utils::Matrix& mat);
	public:
		void init(Device::GxDevicePtr device);
		void cameraChanged(CameraPtr camera);

		void attachToEffect(EffectPtr e);
	};

	SHARED_TYPE(GlobalBufferManager);
}