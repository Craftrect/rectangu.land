#pragma once

#include "GxDevice.h"
#include "ApiObject.h"

namespace Graphics
{
	namespace Device
	{
		WEAK_FORWARD(GxDevice);
	}

	enum class BufferType
	{
		Vertex,
		Index,
		Const,
	};

	class Buffer : public ApiObject
	{
	protected:
		BufferType mBufferType;
		Device::GxDeviceWeakPtr mDevice;

		Buffer(BufferType type, Device::GxDeviceWeakPtr dev, ApiType apitype);

		virtual void setBufferData(const void* data, uint32 numBytes) = 0;

	public:
		template<typename T>
		void setBufferData(const T& value) {
			setBufferData(&value, sizeof(T));
		}

		template<typename T, uint32 size>
		void setBufferData(T(&data)[size]) {
			setBufferData(data, size * sizeof(T));
		}

		template<typename T>
		void setBufferData(const std::vector<T>& data) {
			setBufferData(data.data(), data.size() * sizeof(T));
		}

		virtual void resize(uint32 numBytes) = 0;
	};

	SHARED_TYPE(Buffer);
}