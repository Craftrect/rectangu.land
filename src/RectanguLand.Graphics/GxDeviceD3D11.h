#pragma once

#include "GxDevice.h"

#include "Native.h"
#include "SamplerStateD3D11.h"

namespace Graphics
{
	SHARED_FORWARD(GxPipelineD3D11);
	SHARED_FORWARD(RenderTargetD3D11);

	namespace Device
	{
		//! GxDeviceD3D11 implements GxDevice, provides graphics support
		/*! 
			Provides support for d3d11. Is responsible for native resource
			creation and implements all functions inheritet from GxDevice using
			d3d11 and the dxgi layer
		*/
		class GxDeviceD3D11 : public GxDevice, public std::enable_shared_from_this<GxDeviceD3D11>
		{
			HWND mWindow;

			Utils::ComPtr<ID3D11Device> mDevice;
			Utils::ComPtr<ID3D11DeviceContext> mContext;
			Utils::ComPtr<IDXGISwapChain> mSwapChain;
			Utils::ComPtr<IDXGIAdapter1> mAdapter;
			Utils::ComPtr<IDXGIOutput> mOutput;
			Utils::ComPtr<IDXGIFactory1> mFactory;
			Utils::ComPtr<ID3D11RenderTargetView> mRenderTargetView;
			Utils::ComPtr<ID3D11DepthStencilView> mDepthBufferView;
			Utils::ComPtr<ID3D11Texture2D> mDepthBuffer;
			Utils::ComPtr<ID3D11DepthStencilState> mDepthState;
			Utils::ComPtr<ID3D11DepthStencilState> mNoDepthWriteState;
			Utils::ComPtr<ID3D11RasterizerState> mRasterState;

			SamplerStateD3D11Ptr mDefaultSampler;

#ifdef _DEBUG
			Utils::ComPtr<IDXGIDebug> mDebugInterface;
#endif

			D3D11_TEXTURE2D_DESC mDepthBufferDesc;

			DXGI_SWAP_CHAIN_DESC mChainDesc;
			D3D_FEATURE_LEVEL mFeatureLevel;

			GxPipelineD3D11Ptr mPipeline;

			std::vector<ID3D11RenderTargetView*> mActiveTargets;
			std::vector<RenderTargetD3D11Ptr> mCustomTargets;

			//! Initializes the d3d11 device and all the objects needed
			/*!
				\return	Boolean indicating if the loaded device exactly matches the specifications.
						If false is returned some adjustments have been made (like the device was not
						really loaded with d3d11_feature_level_11_0. It is safe to continue but the
						user can be informed.

				\throws GraphicsException Unable to create the device or any of its objects. Execution
						cannot continue after this error.
			*/
			bool initDefault(bool fullScreen);

			bool fillBufferInformation();
			void fillMultisample();
			void initDefaultResources();

			void initDefaultObjects();

		public:
			~GxDeviceD3D11();

			//! Initializes the d3d11 device and all the objects needed
			/*!
			\return	Boolean indicating if the loaded device exactly matches the specifications.
			If false is returned some adjustments have been made (like the device was not
			really loaded with d3d11_feature_level_11_0. It is safe to continue but the
			user can be informed.

			\throws GraphicsException Unable to create the device or any of its objects. Execution
			cannot continue after this error.
			*/
			bool initDevice(RenderTargetEntryPtr target, bool fullScreen);
			void beginScene();
			void endScene();
			void present();

			VertexBufferPtr createVertexBuffer();
			IndexBufferPtr createIndexBuffer();
			ShaderCodePtr createShaderCode();
			ShaderObjectPtr createShaderObject();
			BufferPtr createBuffer(BufferType type);
			SamplerStatePtr createSampler();
			SamplerStatePtr getDefaultSampler() { return mDefaultSampler; }
			RenderTargetPtr createRenderTarget(RenderTargetType type, int32 width = -1, int32 height = -1);
			TexturePtr createRGBATexture(uint32 width, uint32 height);
			TexturePtr createFloat4Texture(uint32 width, uint32 height);

			void getBackbufferDimension(uint32& width, uint32& height);

			void addRenderTarget(RenderTargetPtr target);
			void setRenderTarget(uint32 index, RenderTargetPtr target);
			void clearCustomRenderTargets();
			void removeRenderTarget(uint32 index);
			void popRenderTarget();
			void removeRenderTarget(RenderTargetPtr target);
			void disableDepthWrite();
			void enableDepthWrite();

			void createBuffer(ID3D11Buffer** ppBuffer, const D3D11_BUFFER_DESC& desc, const void* data = nullptr);

			const Utils::ComPtr<ID3D11DeviceContext>& getImmContext() const { return mContext; }
			Utils::ComPtr<ID3D11DeviceContext> getImmContext() { return mContext; }

			const Utils::ComPtr<ID3D11Device>& getDevice() const { return mDevice; }
			Utils::ComPtr<ID3D11Device> getDevice() { return mDevice; }

			GxPipelinePtr getPipeline();
		};

		SHARED_TYPE(GxDeviceD3D11);
		WEAK_TYPE(GxDeviceD3D11);
	};
};