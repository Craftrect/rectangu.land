#pragma once

#include "RenderTarget.h"
#include "GxDevice.h"
#include "Objects/Texture.h"
#include "InputGeometry.h"
#include "Objects/Buffer.h"

namespace Graphics
{
	SHARED_FORWARD(Camera);

	class SSAO
	{
		static const unsigned int KernelSize = 16;

		// Target0 -> Stores the old color data
		RenderTargetPtr mSsaoTarget;
		// Target1 -> Stores linear depth and normal
		RenderTargetPtr mBufferTarget;

		// Target0, Pass 2 -> Store the actual occlusion
		RenderTargetPtr mOcclusionTarget;

		TexturePtr mRandTexture;
		InputGeometryPtr mGeometry;
		EffectPtr mEffect;
		EffectPtr mEffectPass2;
		BufferPtr mInfoBuffer;

		Device::GxDeviceWeakPtr mDevice;
		bool mIsCapturing;

		struct SSAOBuffer
		{
			Utils::Matrix invProj;
			Utils::Matrix proj;
			Utils::Vector4 sampleKernel[KernelSize];
			float noiseScaleX, noiseScaleY;
			float radius;
			float unused;
		};

		SSAOBuffer mSSAOInfo;

		void initKernel();
		void matrixChanged(CameraPtr cam, const Utils::Matrix& mat);
	public:
		SSAO();

		void init(Device::GxDevicePtr device);
		
		void beginCapture();
		void endCapture();
		void output();

		bool isCapturing() const { return mIsCapturing; }
	};
}