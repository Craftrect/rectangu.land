#include "Precompiled.h"
#include "EffectCollection.h"

namespace Content
{
	void EffectCollection::initEffects() {
		Graphics::EffectPtr effect = std::make_shared<Graphics::Effect>(sWorldFrame->getDevice());
		effect->setShader(Graphics::ShaderCodeType::Vertex, "shaders/vs_5_0/Block.bsx");
		effect->setShader(Graphics::ShaderCodeType::Pixel, "shaders/ps_5_0/Block.bsx");
		effect->setShader(Graphics::ShaderCodeType::Geometry, "shaders/gs_5_0/Block.bsx");
		effect->getParamManager(Graphics::ShaderCodeType::Pixel)->setSampler(0, sWorldFrame->getDevice()->getDefaultSampler());

		mEffects[Effects::Block] = effect;

		sWorldFrame->getGlobalBuffers()->attachToEffect(effect);

		effect = std::make_shared<Graphics::Effect>(sWorldFrame->getDevice());
		effect->setShader(Graphics::ShaderCodeType::Vertex, "shaders/vs_5_0/SSAO.bsx");
		effect->setShader(Graphics::ShaderCodeType::Pixel, "shaders/ps_5_0/SSAO.bsx");
		effect->getParamManager(Graphics::ShaderCodeType::Pixel)->setSampler(0, sWorldFrame->getDevice()->getDefaultSampler());

		mEffects[Effects::SSAO] = effect;

		effect = std::make_shared<Graphics::Effect>(sWorldFrame->getDevice());
		effect->setShader(Graphics::ShaderCodeType::Vertex, "shaders/vs_5_0/SSAO.bsx");
		effect->setShader(Graphics::ShaderCodeType::Pixel, "shaders/ps_5_0/SSAO_Pass2.bsx");
		effect->getParamManager(Graphics::ShaderCodeType::Pixel)->setSampler(0, sWorldFrame->getDevice()->getDefaultSampler());

		mEffects[Effects::SSAO2] = effect;
	}

	Graphics::EffectPtr EffectCollection::getEffect(Effects effect) {
		auto itr = mEffects.find(effect);
		if (itr == mEffects.end()) {
			throw std::invalid_argument("effect is not loaded in the effect collection");
		}

		return itr->second;
	}
}