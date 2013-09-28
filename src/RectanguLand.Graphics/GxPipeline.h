#pragma once

#include "DrawManager.h"
#include "InputGeometry.h"

namespace Graphics
{
	SHARED_FORWARD(InputGeometry);

	enum class BlendMode
	{
		One,
		Zero,
		SrcColor,
		SrcAlpha,
		InvSrcColor,
		InvSrcAlpha,
		DstColor,
		DstAlpha,
		InvDstColor,
		InvDstAlpha
	};

	class GxPipeline
	{
	protected:
		GxPipeline();

		void defaultExecution();
	public:
		virtual void enableDepth() = 0;
		virtual void disableDepth() = 0;
		virtual void setBlend(BlendMode sourceBlend, BlendMode destBlend) = 0;

		void disableBlend() { setBlend(BlendMode::One, BlendMode::Zero); }

		virtual void execute(InputGeometryPtr geometry) = 0;
	};

	SHARED_TYPE(GxPipeline);
};