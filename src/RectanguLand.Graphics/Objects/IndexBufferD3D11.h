#pragma once

#include "IndexBuffer.h"
#include "GxDeviceD3D11.h"

namespace Graphics
{
	class IndexBufferD3D11 : public IndexBuffer
	{
		Device::GxDeviceD3D11Ptr mDevice;
		Utils::ComPtr<ID3D11Buffer> mBuffer;

		D3D11_BUFFER_DESC mBufferDesc;

	private:
		void recreateBuffer(const void* initialData);

	protected:
		void setData(const void* data, uint32 numBytes);

	public:
		IndexBufferD3D11(Device::GxDeviceD3D11Ptr device);

		Utils::ComPtr<ID3D11Buffer> getBufferPtr() const { return mBuffer; }
	};
}