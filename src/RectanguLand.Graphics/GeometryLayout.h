#pragma once

#include "ApiObject.h"
#include "VertexElement.h"
#include "GxDevice.h"
#include "Effect.h"

namespace Graphics
{
	namespace Device
	{
		WEAK_FORWARD(GxDevice);
	}

	SHARED_FORWARD(Effect);

	class GeometryLayout : public ApiObject
	{
	protected:
		GeometryLayout(ApiType type) : ApiObject(type) { }

	public:
		virtual void compile(Device::GxDeviceWeakPtr device,  EffectPtr effect, std::vector<VertexElementPtr>& elements) = 0;
	};

	SHARED_TYPE(GeometryLayout);
}