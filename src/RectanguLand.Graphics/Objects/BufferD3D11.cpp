#include "Precompiled.h"
#include "BufferD3D11.h"
#include "GxDeviceD3D11.h"

namespace Graphics
{
	BufferD3D11::BufferD3D11(BufferType type, Device::GxDeviceWeakPtr dev) : Buffer(type, dev, ApiType::D3D11) {
		if (mDevice.lock()->getType() != ApiType::D3D11)  {
			throw std::invalid_argument("Invalid device specified (should be d3d11)");
		}
	}

	void BufferD3D11::setBufferData(const void* data, uint32 size) {
		if (mBuffer == nullptr) {
			initBuffer(data, size);
		} else {
			auto devptr = mDevice.lock();
			Device::GxDeviceD3D11Ptr dev = std::static_pointer_cast<Device::GxDeviceD3D11>(devptr);
			dev->getImmContext()->UpdateSubresource(mBuffer, 0, nullptr, data, size, 0);
		}
	}

	void BufferD3D11::requireBuffer() {
		if (mBuffer == nullptr) {
			initBuffer(nullptr, 0);
		}
	}

	void BufferD3D11::resize(uint32 numBytes) {
		if (mBuffer != nullptr) {
			if (numBytes < mBufferDesc.ByteWidth) {
				D3D11_BOX dstBox = { 0 };
				dstBox.right = numBytes;
				dstBox.bottom = 1;
				dstBox.back = 1;

				mBufferDesc.ByteWidth = numBytes;
				ID3D11Buffer* tmp = nullptr;
				auto devptr = mDevice.lock();
				Device::GxDeviceD3D11Ptr dev = std::static_pointer_cast<Device::GxDeviceD3D11>(devptr);

				auto res = dev->getDevice()->CreateBuffer(&mBufferDesc, nullptr, &tmp);
				if (FAILED(res)) {
					throw GraphicsException("Failed to create buffer");
				}

				dev->getImmContext()->CopySubresourceRegion(tmp, 0, 0, 0, 0, mBuffer, 0, &dstBox);
				mBuffer = tmp;
			} else if (numBytes > mBufferDesc.ByteWidth) {
				mBufferDesc.ByteWidth = numBytes;
				ID3D11Buffer* tmp = nullptr;
				auto devptr = mDevice.lock();
				Device::GxDeviceD3D11Ptr dev = std::static_pointer_cast<Device::GxDeviceD3D11>(devptr);

				auto res = dev->getDevice()->CreateBuffer(&mBufferDesc, nullptr, &tmp);
				if (FAILED(res)) {
					throw GraphicsException("Failed to create buffer");
				}
				
				dev->getImmContext()->CopySubresourceRegion(tmp, 0, 0, 0, 0, mBuffer, 0, nullptr);
				mBuffer = tmp;
			}
		} else {
			initBuffer(nullptr, numBytes);
		}
	}

	void BufferD3D11::initBuffer(const void* data, uint32 size) {
		memset(&mBufferDesc, 0, sizeof(mBufferDesc));
		
		switch (mBufferType) {
		case BufferType::Const:
			mBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			break;

		case BufferType::Index:
			mBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			break;

		case BufferType::Vertex:
			mBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			break;
		}

		mBufferDesc.ByteWidth = size;
		mBufferDesc.CPUAccessFlags = 0;
		mBufferDesc.MiscFlags = 0;
		mBufferDesc.StructureByteStride = 0;
		mBufferDesc.Usage = D3D11_USAGE_DEFAULT;

		auto devptr = mDevice.lock();
		Device::GxDeviceD3D11Ptr dev = std::static_pointer_cast<Device::GxDeviceD3D11>(devptr);
		D3D11_SUBRESOURCE_DATA resData;
		resData.pSysMem = data;
		resData.SysMemPitch = size;
		resData.SysMemSlicePitch = 0;

		auto res = dev->getDevice()->CreateBuffer(&mBufferDesc, data != nullptr ? &resData : nullptr, mBuffer);
		if (FAILED(res)) {
			throw GraphicsException("Failed to create buffer");
		}
	}
}