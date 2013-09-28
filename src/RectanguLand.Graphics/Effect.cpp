#include "Precompiled.h"
#include "Effect.h"

#include <RectanguLand.Content/AssetManager.h>

namespace Graphics
{
	Effect::Effect(Device::GxDeviceWeakPtr dev) {
		mDevice = dev;

		for (uint32 i = 0; i < (uint32)ShaderCodeType::MAX; ++i) {
			mManagers[i] = std::make_shared<EffectParamManager>();
		}
	}

	void Effect::setShader(ShaderCodeType type, const String& resName) {
		auto code = sAssets->getShaderCode(resName);
		if (code->getCodeType() != type) {
			throw std::invalid_argument("Resource type does not match code type");
		}

		auto shaderObj = mDevice.lock()->createShaderObject();
		shaderObj->load(code);

		mShaders[type] = shaderObj;
	}

	void Effect::begin() {
		auto dev = mDevice.lock();

		for (auto& pair : mShaders) {
			pair.second->apply(dev);
			mManagers[(uint32)pair.first]->apply(dev, pair.second);
		}
	}

	void Effect::end() {
		auto dev = mDevice.lock();

		for (auto& pair : mShaders) {
			pair.second->remove(dev);
			mManagers[(uint32) pair.first]->remove(dev, pair.second);
		}
	}

	void Effect::EffectParamManager::setBuffer(ShaderVariables var, BufferPtr buffer) {
		mBuffers[var] = buffer;
	}

	void Effect::EffectParamManager::setBuffer(uint32 reg, BufferPtr buffer) {
		mBuffersI[reg] = buffer;
	}

	void Effect::EffectParamManager::setTexture(ShaderVariables var, TexturePtr tex) {
		mTextures[var] = tex;
	}

	void Effect::EffectParamManager::setTexture(uint32 reg, TexturePtr tex) {
		mTexturesI[reg] = tex;
	}

	void Effect::EffectParamManager::setSampler(uint32 index, SamplerStatePtr sampler) {
		mSamplers[index] = sampler;
	}

	void Effect::EffectParamManager::setTexture(ShaderVariables var, RenderTargetPtr rt) {
		mRTTextures[var] = rt;
	}

	void Effect::EffectParamManager::setTexture(uint32 reg, RenderTargetPtr rt) {
		mRTTexturesI[reg] = rt;
	}

	void Effect::EffectParamManager::apply(Device::GxDevicePtr device, Graphics::ShaderObjectPtr obj) {
		for (auto& buffPair : mBuffers) {
			obj->setBuffer(device, buffPair.first, buffPair.second);
		}

		for (auto& texPair : mTextures) {
			obj->setTexture(device, texPair.first, texPair.second);
		}

		for (auto& texPair : mRTTextures) {
			obj->setRTTexture(device, texPair.first, texPair.second);
		}

		for (auto& sampPair : mSamplers) {
			obj->setSampler(device, sampPair.first, sampPair.second);
		}

		for (auto& buffPair : mBuffersI) {
			obj->setBuffer(device, buffPair.first, buffPair.second);
		}

		for (auto& texPair : mTexturesI) {
			obj->setTexture(device, texPair.first, texPair.second);
		}

		for (auto& texPair : mRTTexturesI) {
			obj->setRTTexture(device, texPair.first, texPair.second);
		}
	}

	void Effect::EffectParamManager::remove(Device::GxDevicePtr device, Graphics::ShaderObjectPtr obj) {
		// render targets and unordered access views (anything that can be in- and output) has to be removed
		// so that it can be used as output again.
		for (auto& texPair : mRTTextures) {
			obj->setRTTexture(device, texPair.first, nullptr);
		}

		for (auto& texPair : mRTTexturesI) {
			obj->setRTTexture(device, texPair.first, nullptr);
		}
	}

	Effect::EffectParamManagerPtr Effect::getParamManager(ShaderCodeType type) const {
		if (type >= ShaderCodeType::MAX) {
			throw std::invalid_argument("type is not a valid shader code");
		}

		return mManagers[(uint32)type];
	}

	ShaderObjectPtr Effect::getShader(ShaderCodeType type) const {
		auto itr = mShaders.find(type);
		if (itr == mShaders.end()) {
			return nullptr;
		}

		return itr->second;
	}
}