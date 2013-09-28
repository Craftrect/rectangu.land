#pragma once

namespace Content
{
	enum class Effects
	{
		Block,
		SSAO,
		SSAO2,
	};

	class EffectCollection
	{
		std::map<Effects, Graphics::EffectPtr> mEffects;
	public:
		Graphics::EffectPtr getEffect(Effects effect);

		void initEffects();
	};
}