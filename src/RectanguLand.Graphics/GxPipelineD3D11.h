#pragma once

#include "GxPipeline.h"
#include "GxDeviceD3D11.h"

namespace Graphics
{
	enum IndexType;

	class GxPipelineD3D11 : public GxPipeline
	{
		Device::GxDeviceD3D11WeakPtr mDevice;

		DXGI_FORMAT mapIndexFormat(IndexType type);
	public:
		GxPipelineD3D11(Device::GxDeviceD3D11WeakPtr device);

		void enableDepth();
		void disableDepth();
		void setBlend(BlendMode sourceBlend, BlendMode destBlend);
		void execute(InputGeometryPtr geometry);
	};

	SHARED_TYPE(GxPipelineD3D11);
};