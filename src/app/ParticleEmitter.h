/*============================================================
*	@file	 : ParticleEmitter.h
*	@brief	 : パーティクルエミッタ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/06/18
*	@updated : 2026/10/09
*============================================================*/
#pragma once

#include "GameObject.h"
#include "Particle.h"
#include "ParticleBase.h"
#include <memory>

/*------------------------------------------------------------
	初期化用データ構造体
------------------------------------------------------------*/
struct PARTICLE_DESC {
	// 散布方向
	Vector3 Velocity{ 0.0f, 10.0f, 0.0f };

	// 散布率
	Vector3 SpreadRate{ 20.0f, 20.0f, 20.0f };
	
	// 加速度
	Vector3 Accel{};

	// スケール倍率
	float Scale{ 5.0f };

	// 重力
	float Gravity{ 9.8f };

	// 抵抗
	float Drag{ -1.0f };

	// 全体フレーム
	int Life{ 60 };
};

/*------------------------------------------------------------
	前方宣言
------------------------------------------------------------*/
class Timer;

/*============================================================
*	@class	: ParticleEmitter
*	@brief	: パーティクルエミッタ
*============================================================*/
class ParticleEmitter : public GameObject
{
	friend class ParticleRenderer;

private:
	// パーティクル最大数
	static constexpr int PARTICLE_MAX{ 10000 };

	// パーティクル配列
	std::vector<Particle> mParticles{};

	// 初期化用データ構造体
	PARTICLE_DESC mDesc{};

	// 発射インターバル
	double mMaxInterval{ 0.1 };
	double mCurrentInterval{};

	// 一度あたりの発射数
	int mCount{ 100 };

	// ループフラグ
	bool mLoop{ true };

	// エミッタ本体の寿命
	Timer* _mEmitterLife{ nullptr };

	// パーティクルタイプ
	std::unique_ptr<ParticleType::Base> _mType{};

public:	
	ParticleEmitter() = default;

	void Initialize() override;
	void Finalize() override;
	void Update(double deltaTime) override;
	void Draw() const override;

	// ゲッター
	ParticleType::Base* GetType() const { return _mType.get(); }
	int GetParticleMax() const { return PARTICLE_MAX; }
	std::vector<Particle>& GetParticles() { return mParticles; }
	int GetCount() const { return mCount; }
	PARTICLE_DESC GetDesc() const { return mDesc; }
	int GetLife() const { return  mDesc.Life; }

	// セッター
	void SetVelocity(const Vector3& velocity) { mDesc.Velocity = velocity; }
	void SetSpreadRate(const Vector3& spreadRate) { mDesc.SpreadRate = spreadRate; }
	void SetAccel(const Vector3& accel) { mDesc.Accel = accel; }
	void SetScale(const float& scale) { mDesc.Scale = scale; }
	void SetGravity(const float& gravity) { mDesc.Gravity = gravity; }
	void SetDrag(const float& drag) { mDesc.Drag = drag; }
	void SetLife(const int& life) { mDesc.Life = life; }
	void SetCount(const int& count) { mCount = count; }
	void SetInterval(const double& interval) { mMaxInterval = mCurrentInterval = interval; }
	void SetType(std::unique_ptr<ParticleType::Base> type) { _mType = std::move(type); }

	// CSVファイル読み込み
	ParticleEmitter* LoadCSV(const char* filePath);

	// エミッタ寿命のセット
	ParticleEmitter* SetEmitterLife(double lifeTime);

private:
	// 透明度更新
	void alphaUpdate();
};