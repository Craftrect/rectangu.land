#include "Precompiled.h"
#include "GxDeviceD3D11.h"
#include "GxPipelineD3D11.h"
#include "Objects/VertexBufferD3D11.h"
#include "Objects/IndexBufferD3D11.h"
#include "Objects/ShaderCodeD3D11.h"
#include "ShaderObjectD3D11.h"
#include "Objects/BufferD3D11.h"
#include "RenderTargetD3D11.h"
#include "Objects/TextureD3D11.h"

namespace Graphics
{
	namespace Device
	{
		GxDeviceD3D11::~GxDeviceD3D11() {
			mDepthBuffer = nullptr;
			mDepthBufferView = nullptr;
			mRenderTargetView = nullptr;
			mFactory = nullptr;
			mOutput = nullptr;
			mAdapter = nullptr;
			mContext = nullptr;
			mDevice = nullptr;
			mSwapChain = nullptr;

#ifdef _DEBUG
			if (mDebugInterface != nullptr) {
				mDebugInterface->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			}
#endif
		}

		bool GxDeviceD3D11::initDevice(RenderTargetEntryPtr target, bool fullScreen) {
#ifdef _DEBUG
			typedef HRESULT(WINAPI* tDXGIGetDebugInterface)(REFIID iid, void** ppUnk);
			static tDXGIGetDebugInterface DXGIGetDebugInterface = nullptr;

			if (DXGIGetDebugInterface == nullptr) {
				HMODULE hDXGIDebug = LoadLibrary(L"dxgidebug.dll");
				DXGIGetDebugInterface = (tDXGIGetDebugInterface) GetProcAddress(hDXGIDebug, "DXGIGetDebugInterface");
			}

			if (DXGIGetDebugInterface != nullptr) {
				IDXGIDebug** ppDebug = mDebugInterface;
				if (FAILED(DXGIGetDebugInterface(IID_PPV_ARGS(ppDebug)))) {
					mDebugInterface = nullptr;
				}
			}
#endif
			auto handle = target->getProperty(RenderTargetEntry::Properties::WindowHandle);
			if (handle.empty()) {
				throw GraphicsException(_T("Rendertarget is not a valid window"));
			}

			HWND window = nullptr;
			try {
				window = boost::any_cast<HWND>(handle);
				if (IsWindow(window) == FALSE) {
					throw GraphicsException(_T("Rendertarget is not a valid window"));
				}

				mWindow = window;
			} catch (boost::bad_any_cast) {
				throw GraphicsException(_T("Rendertarget is not a valid window"));
			}

			return initDefault(fullScreen);
		}

		bool GxDeviceD3D11::initDefault(bool fullScreen) {
			IDXGIFactory1* factory = nullptr;

			IDXGIFactory1** fac = mFactory;
			auto result = CreateDXGIFactory1(IID_PPV_ARGS(fac));
			if (FAILED(result)) {
				throw GraphicsException(_T("Could not create dxgi factory"));
			}

			result = mFactory->EnumAdapters1(0, mAdapter);
			if (FAILED(result)) {
				throw GraphicsException(_T("Could not obtain adapter from factory"));
			}

			result = mAdapter->EnumOutputs(0, mOutput);
			if (FAILED(result)) {
				throw GraphicsException(_T("Could not obtain output ffrom adapter"));
			}

			memset(&mChainDesc, 0, sizeof(mChainDesc));

			mChainDesc.BufferCount = 1;
			mChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			mChainDesc.Flags = 0;
			mChainDesc.OutputWindow = mWindow;
			mChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			mChainDesc.Windowed = fullScreen ? FALSE : TRUE;

			bool exactMatch = fillBufferInformation();
			fillMultisample();

			D3D_FEATURE_LEVEL levels [] = {
				D3D_FEATURE_LEVEL_11_0,
			};

			result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
				/*D3D11_CREATE_DEVICE_DEBUG*/0, levels, 1, D3D11_SDK_VERSION, &mChainDesc, mSwapChain, mDevice, &mFeatureLevel, mContext);

			if (FAILED(result)) {
				throw GraphicsException(_T("Could not create device & swapchain"));
			}

			mDevice->GetImmediateContext(mContext);

			initDefaultResources();

			mPipeline = std::make_shared<GxPipelineD3D11>(shared_from_this());

			mCustomTargets.reserve(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
			mActiveTargets.reserve(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);

			mCustomTargets.push_back(nullptr);

			return exactMatch;
		}

		bool GxDeviceD3D11::fillBufferInformation() {
			memset(&mChainDesc.BufferDesc, 0, sizeof(DXGI_MODE_DESC));

			auto& mode = mChainDesc.BufferDesc;

			bool exactMatch = true;

			mode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			if (mChainDesc.Windowed == FALSE) {
				HWND hDesktop = GetDesktopWindow();
				RECT desktopRect = { 0 };
				GetWindowRect(hDesktop, &desktopRect);

				uint32 width = desktopRect.right - desktopRect.left;
				uint32 height = desktopRect.bottom - desktopRect.top;

				mode.Height = height;
				mode.Width = width;
				mode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
				mode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				mode.RefreshRate.Numerator = 60;
				mode.RefreshRate.Denominator = 1;

				DXGI_MODE_DESC match = { 0 };
				auto result = mOutput->FindClosestMatchingMode(&mode, &match, nullptr);
				if (FAILED(result)) {
					throw GraphicsException(_T("Unable to find a matching display mode for fullscreen"));
				}

				mChainDesc.BufferDesc = match;
				exactMatch = match.Width == width && match.Height == height;
			} else {
				RECT wndRect = { 0 };
				GetWindowRect(mWindow, &wndRect);

				uint32 width = wndRect.right - wndRect.left;
				uint32 height = wndRect.bottom - wndRect.top;

				mode.Height = height;
				mode.Width = width;
				mode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
				mode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				mode.RefreshRate.Numerator = 60;
				mode.RefreshRate.Denominator = 1;

				auto result = mOutput->FindClosestMatchingMode(&mode, &mChainDesc.BufferDesc, nullptr);
				if (FAILED(result)) {
					throw GraphicsException(_T("Unable to find a matching display mode for fullscreen"));
				}

				mChainDesc.BufferDesc.Width = width;
				mChainDesc.BufferDesc.Height = height;
				mChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				mChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
				exactMatch = true;
			}

			return exactMatch;
		}

		void GxDeviceD3D11::fillMultisample() {
			mChainDesc.SampleDesc.Count = 1;
			mChainDesc.SampleDesc.Quality = 0;
		}

		void GxDeviceD3D11::beginScene() {
			static float background[] = { 0, 0, 0, 1 };

			mContext->ClearRenderTargetView(mRenderTargetView, background);
			mContext->ClearDepthStencilView(mDepthBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		}

		void GxDeviceD3D11::endScene() {

		}

		void GxDeviceD3D11::present() {
			mSwapChain->Present(0, 0);
		}

		void GxDeviceD3D11::initDefaultResources() {
			ID3D11Texture2D* backBuffer = nullptr;

			auto result = mSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
			if (FAILED(result)) {
				throw GraphicsException(_T("Unable to retreive backbuffer of swapchain"));
			}

			result = mDevice->CreateRenderTargetView(backBuffer, nullptr, mRenderTargetView);
			backBuffer->Release();

			mActiveTargets.push_back(mRenderTargetView);

			if (FAILED(result)) {
				throw GraphicsException(_T("Unable to map view of render target"));
			}

			mDepthBufferDesc.ArraySize = 1;
			mDepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			mDepthBufferDesc.CPUAccessFlags = 0;
			mDepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			mDepthBufferDesc.Height = mChainDesc.BufferDesc.Height;
			mDepthBufferDesc.Width = mChainDesc.BufferDesc.Width;
			mDepthBufferDesc.MipLevels = 1;
			mDepthBufferDesc.MiscFlags = 0;
			mDepthBufferDesc.SampleDesc = mChainDesc.SampleDesc;
			mDepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			
			result = mDevice->CreateTexture2D(&mDepthBufferDesc, nullptr, mDepthBuffer);
			if (FAILED(result)) {
				throw GraphicsException(_T("Unable to create the depth buffer"));
			}

			D3D11_DEPTH_STENCIL_VIEW_DESC dsView;
			dsView.Format = mDepthBufferDesc.Format;
			dsView.Flags = 0;
			dsView.Texture2D.MipSlice = 0;
			dsView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

			result = mDevice->CreateDepthStencilView(mDepthBuffer, &dsView, mDepthBufferView);
			if (FAILED(result)) {
				throw GraphicsException(_T("Unable to map view of depth buffer"));
			}

			mContext->OMSetRenderTargets(mActiveTargets.size(), mActiveTargets.data(), mDepthBufferView);

			D3D11_VIEWPORT viewport;
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;
			viewport.Height = (float)mChainDesc.BufferDesc.Height;
			viewport.Width = (float)mChainDesc.BufferDesc.Width;
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;

			mContext->RSSetViewports(1, &viewport);

			D3D11_RASTERIZER_DESC rasterDesc;
			memset(&rasterDesc, 0, sizeof(rasterDesc));

			rasterDesc.AntialiasedLineEnable = false;
			rasterDesc.CullMode = D3D11_CULL_NONE;
			rasterDesc.DepthBias = 0;
			rasterDesc.DepthBiasClamp = 0.0f;
			rasterDesc.DepthClipEnable = false;
			rasterDesc.FillMode = D3D11_FILL_SOLID;
			rasterDesc.FrontCounterClockwise = false;
			rasterDesc.MultisampleEnable = false;
			rasterDesc.ScissorEnable = false;
			rasterDesc.SlopeScaledDepthBias = 0.0f;

			result = mDevice->CreateRasterizerState(&rasterDesc, mRasterState);
			if (FAILED(result)) {
				throw GraphicsException(_T("Unable to create rasterizer state"));
			}

			mContext->RSSetState(mRasterState);

			D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
			memset(&depthStencilDesc, 0, sizeof(depthStencilDesc));

			depthStencilDesc.DepthEnable                  = true;
			depthStencilDesc.DepthWriteMask               = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc                    = D3D11_COMPARISON_LESS;

			depthStencilDesc.StencilEnable                = false;
			depthStencilDesc.StencilReadMask              = 0xFF;
			depthStencilDesc.StencilWriteMask             = 0xFF;

			depthStencilDesc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			depthStencilDesc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;

			depthStencilDesc.BackFace.StencilFailOp       = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilDepthFailOp  = D3D11_STENCIL_OP_DECR;
			depthStencilDesc.BackFace.StencilPassOp       = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilFunc         = D3D11_COMPARISON_ALWAYS;

			result = mDevice->CreateDepthStencilState(&depthStencilDesc, mDepthState);
			if (FAILED(result)) {
				throw GraphicsException(_T("Unable to create depth state"));
			}

			depthStencilDesc.DepthEnable = false;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

			result = mDevice->CreateDepthStencilState(&depthStencilDesc, mNoDepthWriteState);
			if (FAILED(result)) {
				throw GraphicsException(_T("Unable to create depth state"));
			}

			mContext->OMSetDepthStencilState(mDepthState, 1);

			initDefaultObjects();
		}

		void GxDeviceD3D11::initDefaultObjects() {
			mDefaultSampler = std::dynamic_pointer_cast<SamplerStateD3D11>(createSampler());
		}

		GxPipelinePtr GxDeviceD3D11::getPipeline() {
			return mPipeline;
		}

		void GxDeviceD3D11::createBuffer(ID3D11Buffer** ppBuffer, const D3D11_BUFFER_DESC& desc, const void* data) {
			D3D11_SUBRESOURCE_DATA subData = { 0 };
			subData.pSysMem = data;
			subData.SysMemPitch = desc.ByteWidth;
			subData.SysMemSlicePitch = 0;

			auto result = mDevice->CreateBuffer(&desc, &subData, ppBuffer);
			if (FAILED(result)) {
				throw GraphicsException(_T("Could not create buffer object"));
			}
		}

		VertexBufferPtr GxDeviceD3D11::createVertexBuffer() {
			return std::make_shared<VertexBufferD3D11>(shared_from_this());
		}

		IndexBufferPtr GxDeviceD3D11::createIndexBuffer() {
			return std::make_shared<IndexBufferD3D11>(shared_from_this());
		}

		ShaderCodePtr GxDeviceD3D11::createShaderCode() {
			return std::make_shared<ShaderCodeD3D11>(shared_from_this());
		}

		ShaderObjectPtr GxDeviceD3D11::createShaderObject() {
			return std::make_shared<ShaderObjectD3D11>();
		}

		BufferPtr GxDeviceD3D11::createBuffer(BufferType type) {
			return std::make_shared<BufferD3D11>(type, shared_from_this());
		}

		SamplerStatePtr GxDeviceD3D11::createSampler() {
			return std::make_shared<SamplerStateD3D11>(shared_from_this());
		}

		RenderTargetPtr GxDeviceD3D11::createRenderTarget(RenderTargetType type, int32 width, int32 height) {
			uint32 w = (width < 0) ? mChainDesc.BufferDesc.Width : (uint32) width;
			uint32 h = (height < 0) ? mChainDesc.BufferDesc.Height : (uint32) height;

			return std::make_shared<RenderTargetD3D11>(shared_from_this(), type, w, h);
		}

		void GxDeviceD3D11::addRenderTarget(RenderTargetPtr target) {
			if (target->getApiType() != ApiType::D3D11) {
				throw std::invalid_argument("render target is not d3d11");
			}

			if (mActiveTargets.size() >= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT) {
				throw std::bad_function_call("Maximum number of rendertargets is set.");
			}

			auto tar11 = std::static_pointer_cast<RenderTargetD3D11>(target);
			mCustomTargets.push_back(tar11);
			mActiveTargets.push_back(tar11->getRTView());

			mContext->OMSetRenderTargets(mActiveTargets.size(), mActiveTargets.data(), mDepthBufferView);
		}

		void GxDeviceD3D11::setRenderTarget(uint32 index, RenderTargetPtr target) {
			if (target->getApiType() != ApiType::D3D11) {
				throw std::invalid_argument("render target is not d3d11");
			}

			if (index >= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT) {
				throw std::invalid_argument("index is out of range of maximum number of rendertargets");
			}

			auto tar11 = std::static_pointer_cast<RenderTargetD3D11>(target);

			if (index >= mActiveTargets.size()) {
				for (uint32 i = mActiveTargets.size(); i <= index; ++i) {
					mActiveTargets.push_back(nullptr);
					mCustomTargets.push_back(nullptr);
				}
			}

			mActiveTargets[index] = tar11->getRTView();
			mCustomTargets[index] = tar11;

			mContext->OMSetRenderTargets(mActiveTargets.size(), mActiveTargets.data(), mDepthBufferView);
		}

		void GxDeviceD3D11::clearCustomRenderTargets() {
			mActiveTargets.clear();
			mCustomTargets.clear();
			mCustomTargets.push_back(nullptr);
			mActiveTargets.push_back(mRenderTargetView);
			mContext->OMSetRenderTargets(mActiveTargets.size(), mActiveTargets.data(), mDepthBufferView);
		}

		void GxDeviceD3D11::removeRenderTarget(uint32 index) {
			if (index >= mCustomTargets.size()) {
				throw std::out_of_range("Invalid render target index");
			}

			// mCustomTargets[0] is the main render target (color output)
			// if its nullptr the default render target is set, removing
			// it should not have any effect.
			if (index == 0 && mCustomTargets[0] == nullptr) {
				return;
			}

			if (index == 0) {
				mActiveTargets[0] = mRenderTargetView;
				mCustomTargets[0] = nullptr;
			} else {
				mCustomTargets.erase(mCustomTargets.begin() + index);
				mActiveTargets.erase(mActiveTargets.begin() + index);

				while (mCustomTargets.size() != 0 && mCustomTargets.back() == nullptr) {
					mCustomTargets.pop_back();
					mActiveTargets.pop_back();
				}

				if (mCustomTargets.size() == 0) {
					mCustomTargets.push_back(nullptr);
					mActiveTargets.push_back(mRenderTargetView);
				}
			}

			mContext->OMSetRenderTargets(mActiveTargets.size(), mActiveTargets.data(), mDepthBufferView);
		}

		void GxDeviceD3D11::popRenderTarget() {
			// there is no custom render target currently bound, only the default one
			// so poping one is a nono
			if (mCustomTargets.size() == 1 && mCustomTargets[0] == nullptr) {
				return;
			}

			mActiveTargets.pop_back();
			mCustomTargets.pop_back();

			if (mActiveTargets.size() == 0) {
				mActiveTargets.push_back(mRenderTargetView);
				mCustomTargets.push_back(nullptr);
			}

			mContext->OMSetRenderTargets(mActiveTargets.size(), mActiveTargets.data(), mDepthBufferView);
		}

		void GxDeviceD3D11::removeRenderTarget(RenderTargetPtr target) {
			if (target->getApiType() != ApiType::D3D11) {
				throw std::invalid_argument("render target is not d3d11");
			}

			auto tar11 = std::static_pointer_cast<RenderTargetD3D11>(target);

			uint32 index = 0;
			for (auto& rt : mCustomTargets) {
				if (rt == target) {
					removeRenderTarget(index);
					return;
				}
			}
		}

		TexturePtr GxDeviceD3D11::createRGBATexture(uint32 width, uint32 height) {
			Utils::ComPtr<ID3D11Texture2D> texture;

			D3D11_TEXTURE2D_DESC desc;
			desc.ArraySize = 1;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.Height = height;
			desc.MipLevels = 1;
			desc.MiscFlags = 0;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.Width = width;

			auto res = mDevice->CreateTexture2D(&desc, nullptr, texture);
			if (FAILED(res)) {
				throw GraphicsException("Unable to create texture");
			}

			auto ret = std::make_shared<TextureD3D11>(shared_from_this());
			ret->setRealData(texture, false, 1);

			return ret;
		}

		TexturePtr GxDeviceD3D11::createFloat4Texture(uint32 width, uint32 height) {
			Utils::ComPtr<ID3D11Texture2D> texture;

			D3D11_TEXTURE2D_DESC desc;
			desc.ArraySize = 1;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			desc.Height = height;
			desc.MipLevels = 1;
			desc.MiscFlags = 0;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.Width = width;

			auto res = mDevice->CreateTexture2D(&desc, nullptr, texture);
			if (FAILED(res)) {
				throw GraphicsException("Unable to create texture");
			}

			auto ret = std::make_shared<TextureD3D11>(shared_from_this());
			ret->setRealData(texture, false, 1);

			return ret;
		}

		void GxDeviceD3D11::getBackbufferDimension(uint32& width, uint32& height) {
			width = mChainDesc.BufferDesc.Width;
			height = mChainDesc.BufferDesc.Height;
		}

		void GxDeviceD3D11::disableDepthWrite() {
			mContext->OMSetDepthStencilState(mNoDepthWriteState, 1);
		}

		void GxDeviceD3D11::enableDepthWrite() {
			mContext->OMSetDepthStencilState(mDepthState, 1);
		}
	};
};