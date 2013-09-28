#pragma once

#include "Buffer.h"
#include "Native.h"

namespace Graphics
{
	class BufferD3D11 : public Buffer
	{
	private:
		Utils::ComPtr<ID3D11Buffer> mBuffer;
		D3D11_BUFFER_DESC mBufferDesc;

		void requireBuffer();
		void initBuffer(const void* data, uint32 size);

	protected:
		void setBufferData(const void* data, uint32 numBytes);

	public:
		BufferD3D11(BufferType type, Device::GxDeviceWeakPtr dev);
		void resize(uint32 numBytes);

		ID3D11Buffer* getBuffer() { return mBuffer; }
	};

	SHARED_TYPE(BufferD3D11);
}