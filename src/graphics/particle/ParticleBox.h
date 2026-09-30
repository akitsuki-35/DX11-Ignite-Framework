/*============================================================
*	@file	 : ParticleBox.h
*	@brief	 : ボックス型散布パーティクル
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/27
*	@updated : 2026/09/30
*============================================================*/
#pragma once

#include "ParticleBase.h"

namespace ParticleType {
/*============================================================
*	@class	: ParticleType::Box
*	@brief	: ボックス型散布パーティクル
*============================================================*/
	class Box : public Base
	{
	public:
		Box(ParticleEmitter* emitter)
			: Base(emitter) {}

		void Emission(PARTICLE_DESC& desc) override;

		std::string_view GetTypeName() const override { return "Box"; };
	};
}