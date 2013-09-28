#include "Precompiled.h"
#include "IndexBufferD3D11.h"

namespace Graphics
{
	IndexBufferD3D11::IndexBufferD3D11(Device::GxDeviceD3D11Ptr device) : IndexBuffer(ApiType::D3D11) {
		mDevice = device;

		memset(&mBufferDesc, 0, sizeof(mBufferDesc));
		mBufferDesc.CPUAccessFlags = 0;
		mBufferDesc.StructureByteStride = 0;
		mBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		mBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	}

	void IndexBufferD3D11::setData(const void* data, uint32 size) {
		if (size == 0) {
			throw std::invalid_argument("Invalid size for index buffer: 0");
		}

		if (size > mBufferDesc.ByteWidth) {
			mBufferDesc.ByteWidth = size;

			recreateBuffer(data);
		} else if (size < mBufferDesc.ByteWidth) {
			D3D11_BOX box;
			box.back = box.front = 0;
			box.bottom = 1;
			box.top = 0;
			box.left = 0;
			box.right = size;

			mDevice->getImmContext()->UpdateSubresource(mBuffer, 0, &box, data, size, 0);
		} else {
			mDevice->getImmContext()->UpdateSubresource(mBuffer, 0, nullptr, data, size, 0);
		}
	}

	void IndexBufferD3D11::recreateBuffer(const void* initialData) {
		ID3D11Buffer* buffer = nullptr;

		mDevice->createBuffer(&buffer, mBufferDesc, initialData);

		mBuffer = Utils::ComPtr<ID3D11Buffer>(buffer, false);
	}
}