#pragma once

#include "Defines.h"

namespace Graphics
{
	class ApiObject
	{
	protected:
		ApiType mType;

		ApiObject(ApiType type) : mType(type) {

		}

	public:
		ApiType getApiType() const { return mType; }
	};
}