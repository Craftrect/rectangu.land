#pragma once

namespace Graphics
{
	class RenderTargetEntry
	{
	public:
		enum class Properties
		{
			WindowHandle,
			Width,
			Height,
		};

		virtual boost::any getProperty(Properties prop) const = 0;
		virtual void setProperty(Properties prop, const boost::any& value) = 0;
	};

	SHARED_TYPE(RenderTargetEntry);
};