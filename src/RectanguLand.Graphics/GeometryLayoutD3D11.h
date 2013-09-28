#pragma once

#include "GeometryLayout.h"
#include "Native.h"

namespace Graphics
{
	class GeometryLayoutD3D11 : public GeometryLayout
	{
	private:
		std::vector<D3D11_INPUT_ELEMENT_DESC> mInputElements;
		Utils::ComPtr<ID3D11InputLayout> mLayout;

		DXGI_FORMAT calcFormat(uint32 componentCount, ComponentType type);
	public:
		GeometryLayoutD3D11();
		void compile(Device::GxDeviceWeakPtr device, EffectPtr effect, std::vector<VertexElementPtr>& elements);

		Utils::ComPtr<ID3D11InputLayout> getLayout() const { return mLayout; }
	};

	SHARED_TYPE(GeometryLayoutD3D11);
}