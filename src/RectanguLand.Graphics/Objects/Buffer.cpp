#include "Precompiled.h"
#include "Buffer.h"

namespace Graphics
{
	Buffer::Buffer(BufferType type, Device::GxDeviceWeakPtr dev, ApiType apiType) : ApiObject(apiType), mBufferType(type), mDevice(dev) {
	}
}