/*============================================================
*	@file	 : Particle.h
*	@brief	 : パーティクル
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/15
*	@updated : 2026/09/30
*============================================================*/
#pragma once

#include "Vector3.h"

/*============================================================
*	@class	: Particle
*	@brief	: パーティクルの粒子構造体
*============================================================*/
class Particle
{
	friend class ParticleEmitter;
	friend class ParticleRenderer;

private:
	// 座標
	Vector3 mPosition{ 0.0f, 0.0f, 0.0f };

	// スケール
	Vector3 mScale{ 1.0f, 1.0f, 1.0f };

	// 速度
	Vector3 mVelocity{};
	
	// 加速度
	Vector3 mAccel{};

	// 重力加速度
	float mGravity{};

	// 抵抗
	float mDrag{};

	// 寿命
	int mLife{ 0 };

	// 有効状態
	bool mEnable{ false };

public:
	Particle() = default;

	// 更新
	void Update(double deltaTime);

	// パラメータのセット
	// positionには基本的にエミッタ本体の座標を指定する
	void SetParameter(const Vector3& position, const Vector3& velocity, const Vector3& accel, const Vector3& scale,
		const float& gravity,  const float& drag, const int& life);

	// 有効・無効切り替え
	void SetEnable(bool enable) { mEnable = enable; }

	// 有効・無効状態取得
	bool IsEnable() const { return mEnable; }
};