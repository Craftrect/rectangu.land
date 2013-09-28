#pragma once

namespace Graphics
{
	class IDrawable
	{
	public:
		virtual void draw() = 0;
	};

	SHARED_TYPE(IDrawable);
}