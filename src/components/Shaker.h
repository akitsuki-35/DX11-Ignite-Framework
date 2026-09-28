/*============================================================
*	@file	 : Shaker.h
*	@brief	 : シェイクコンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/09/28
*	@updated : 2026/09/28
*============================================================*/
#pragma once

#include "Component.h"
#include "Vector3.h"
#include "Timer.h"
#include <memory>

/*============================================================
*	@class	: Shaker
*	@brief	: シェイクコンポーネント
*============================================================*/
class Shaker : public Component
{
private:
	// シェイク座標オフセット
	Vector3 mOffset{};

	// シェイク用タイマー
	std::unique_ptr<Timer> _mShakeTimer{ nullptr };

	// シェイク強度
	float mShakePower{};

public:
	Shaker(GameObject* owner)
		: Component(owner) {}

	void Initialize() override;
	void Finalize() override;
	void Update(double deltaTime) override;

	// シェイク開始
	void Shake(float power, double shakeTime = 1.0);

	// シェイク座標オフセット取得
	Vector3 GetShakeOffset();

	// シェイク中？
	bool IsSeeking();
};