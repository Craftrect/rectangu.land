#pragma once

namespace Graphics
{
	class GraphicsException : public std::exception
	{
	public:
		GraphicsException(const String& message) {

		}
	};
};