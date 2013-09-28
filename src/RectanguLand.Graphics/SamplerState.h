#pragma once

#include "ApiObject.h"

namespace Graphics
{
	enum class AddressMode
	{
		Wrap,
		Clamp,
		Mirror,
		Border,
	};

	enum class FilterMode
	{
		Bilinear,
		Trilinear,
		Anisotropic,
	};

	class SamplerState : public ApiObject
	{
	protected:
		SamplerState(ApiType type) : ApiObject(type) {

		}

	public:

		void setAddressMode(AddressMode u, AddressMode v, AddressMode w) { setAddressU(u); setAddressV(v); setAddressW(w); }
		virtual void setAddressU(AddressMode mode) = 0;
		virtual void setAddressV(AddressMode mode) = 0;
		virtual void setAddressW(AddressMode mode) = 0;
		virtual void setFilter(FilterMode mode, float anisotropy = 0.0f) = 0;
	};

	SHARED_TYPE(SamplerState);
}