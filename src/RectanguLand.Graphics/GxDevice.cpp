#include "Precompiled.h"
#include "GxDevice.h"
#include "GxDeviceD3D11.h"

namespace Graphics
{
	namespace Device
	{
		GxDevicePtr GxDevice::createDevice(ApiType type) {
			GxDevicePtr retDevice = nullptr;
			switch (type) {
			case ApiType::D3D11: {
					retDevice = std::make_shared<GxDeviceD3D11>();
				}
				break;

			default:
				throw GraphicsException(L"Device type not supported.");
			}

			retDevice->setType(type);
			return retDevice;
		}
	};
};