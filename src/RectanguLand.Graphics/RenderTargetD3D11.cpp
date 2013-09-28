#include "Precompiled.h"
#include "RenderTargetD3D11.h"
#include "GxDeviceD3D11.h"

namespace Graphics
{
	RenderTargetD3D11::RenderTargetD3D11(Device::GxDeviceWeakPtr device, RenderTargetType type, uint32 width, uint32 height) : RenderTarget(ApiType::D3D11) {
		if (device.lock()->getType() != ApiType::D3D11) {
			throw std::invalid_argument("cannot generate d3d11 rendertarget from non-d3d11 device");
		}

		mDevice = device;

		auto dev11 = std::static_pointer_cast<Device::GxDeviceD3D11>(device.lock());
		
		D3D11_TEXTURE2D_DESC texdesc = { 0 };
		texdesc.ArraySize = 1;
		texdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		texdesc.CPUAccessFlags = 0;
		texdesc.Height = height;
		texdesc.Width = width;
		texdesc.MipLevels = 1;
		texdesc.MiscFlags = 0;
		texdesc.SampleDesc.Count = 1;
		texdesc.SampleDesc.Quality = 0;
		texdesc.Usage = D3D11_USAGE_DEFAULT;

		switch (type) {
		case RenderTargetType::Float:
			texdesc.Format = DXGI_FORMAT_R32_FLOAT;
			break;

		case RenderTargetType::Float2:
			texdesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			break;

		case RenderTargetType::Float3:
			texdesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			break;

		case RenderTargetType::Float4:
			texdesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;

		case RenderTargetType::RGBA:
			texdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;

		case RenderTargetType::R:
			texdesc.Format = DXGI_FORMAT_R8_UNORM;
			break;

		default:
			throw std::invalid_argument("Invalid rendertarget type");
		}

		auto res = dev11->getDevice()->CreateTexture2D(&texdesc, nullptr, mResource);
		if (FAILED(res)) {
			throw GraphicsException("Unable to create rendertarget texture");
		}

		D3D11_RENDER_TARGET_VIEW_DESC rtvd;
		rtvd.Format = texdesc.Format;
		rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvd.Texture2D.MipSlice = 0;
		
		res = dev11->getDevice()->CreateRenderTargetView(mResource, &rtvd, mRTView);
		if (FAILED(res)) {
			throw GraphicsException("Unable to create view of rendertarget");
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		srvd.Format = texdesc.Format;
		srvd.Texture2D.MipLevels = 1;
		srvd.Texture2D.MostDetailedMip = 0;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		res = dev11->getDevice()->CreateShaderResourceView(mResource, &srvd, mSRView);
		if (FAILED(res)) {
			throw GraphicsException("Unable to create view of shader resource");
		}
	}

	void RenderTargetD3D11::clear(const Utils::Vector4& color) {
		auto dev11 = std::static_pointer_cast<Device::GxDeviceD3D11>(mDevice.lock());
		dev11->getImmContext()->ClearRenderTargetView(mRTView, reinterpret_cast<const float*>(&color));
	}
}