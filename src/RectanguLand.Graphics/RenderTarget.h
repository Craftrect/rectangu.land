#pragma once

#include "ApiObject.h"

namespace Graphics
{
	enum RenderTargetType
	{
		Float4,
		Float3,
		Float2,
		Float,
		RGBA,
		R
	};

	class RenderTarget : public ApiObject
	{
	protected:
		RenderTarget(ApiType type) : ApiObject(type) {

		}

	public:
		virtual void clear(const Utils::Vector4& color) = 0;
	};

	SHARED_TYPE(RenderTarget);
}