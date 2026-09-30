/*============================================================
*	@file	 : ParticleBase.h
*	@brief	 : パーティクルタイプ定義
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/19
*	@updated : 2026/09/30
*============================================================*/
#pragma once

#include <string>
#include <string_view>
#include <memory>

/*------------------------------------------------------------
	前方宣言
------------------------------------------------------------*/
class ParticleEmitter;
struct PARTICLE_DESC;

/*============================================================
*	@namespace	: ParticleType
*	@brief		: パーティクルタイプ定義
*============================================================*/
namespace ParticleType {
/*============================================================
*	@class	: ParticleType::Base
*	@brief	: パーティクル形状基底クラス
*============================================================*/
	class Base
	{
		friend ParticleEmitter;

	protected:
		ParticleEmitter* _mEmitter{};

	public:
		Base(ParticleEmitter* emitter) 
			:_mEmitter(emitter) {}
		virtual ~Base() = default;

		// パーティクル発射
		virtual void Emission(PARTICLE_DESC& desc) = 0;

		// 発射後パーティクルの更新
		virtual void Update(double deltaTime);

		// CSVファイル読み込み・書き出し
		virtual std::unique_ptr<ParticleType::Base> LoadCSV(const char* filePath);
		virtual std::string_view GetTypeName() const = 0;
	};
}