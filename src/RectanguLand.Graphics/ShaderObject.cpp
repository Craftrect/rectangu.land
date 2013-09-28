#include "Precompiled.h"
#include "ShaderObject.h"

namespace Graphics
{
	ShaderObject::ShaderObject() {
		mIsLoaded = false;
		mType = ShaderCodeType::Pixel;
	}
}