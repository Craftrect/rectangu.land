#pragma once

#include "RenderTargetEntry.h"
#include "GxPipeline.h"
#include "Objects/VertexBuffer.h"
#include "Objects/IndexBuffer.h"
#include "ShaderCode.h"
#include "ShaderObject.h"
#include "Defines.h"
#include "Objects/Buffer.h"
#include "SamplerState.h"
#include "RenderTarget.h"

namespace Graphics
{
	enum class BufferType;
	SHARED_FORWARD(GxPipeline);
	SHARED_FORWARD(Texture);

	namespace Device
	{
		SHARED_FORWARD(GxDevice);
		WEAK_FORWARD(GxDevice);

		class GxDevice
		{
		private:
			ApiType mType;

			void setType(ApiType type) { mType = type; }
		public:
			virtual ~GxDevice() { }
			virtual bool initDevice(RenderTargetEntryPtr renderTarget, bool fullScreen) = 0;
			virtual void beginScene() = 0;
			virtual void endScene() = 0;
			virtual void present() = 0;
			virtual GxPipelinePtr getPipeline() = 0;
			virtual VertexBufferPtr createVertexBuffer() = 0;
			virtual IndexBufferPtr createIndexBuffer() = 0;
			virtual ShaderCodePtr createShaderCode() = 0;
			virtual ShaderObjectPtr createShaderObject() = 0;
			virtual BufferPtr createBuffer(BufferType type) = 0;
			virtual SamplerStatePtr createSampler() = 0;
			virtual SamplerStatePtr getDefaultSampler() = 0;
			virtual RenderTargetPtr createRenderTarget(RenderTargetType type, int32 width = -1, int32 height = -1) = 0;
			virtual void addRenderTarget(RenderTargetPtr target) = 0;
			virtual void setRenderTarget(uint32 index, RenderTargetPtr target) = 0;
			virtual void clearCustomRenderTargets() = 0;
			virtual void removeRenderTarget(uint32 index) = 0;
			virtual void popRenderTarget() = 0;
			virtual void removeRenderTarget(RenderTargetPtr target) = 0;
			virtual TexturePtr createRGBATexture(uint32 width, uint32 height) = 0;
			virtual TexturePtr createFloat4Texture(uint32 width, uint32 height) = 0;
			virtual void getBackbufferDimension(uint32& width, uint32& height) = 0;
			virtual void disableDepthWrite() = 0;
			virtual void enableDepthWrite() = 0;

			ApiType getType() const { return mType; }

			static GxDevicePtr createDevice(ApiType type);
		};
	};
};