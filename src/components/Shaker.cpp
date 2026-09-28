/*============================================================
*	@file	 : Shaker.cpp
*	@brief	 : シェイクコンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/09/28
*	@updated : 2026/09/28
*============================================================*/
#include "Shaker.h"

void Shaker::Initialize()
{
	// タイマー初期化
	_mShakeTimer = std::make_unique<Timer>();
}

void Shaker::Finalize()
{
	_mShakeTimer->Finalize();
	_mShakeTimer = nullptr;
	
	Component::Finalize();
}

void Shaker::Update(double deltaTime)
{
	if (!_mShakeTimer->GetEnable()) return;

	// タイマーの進行度に応じて揺れの強さを算出
	float progress = _mShakeTimer->GetProgress();
	float intensity = mShakePower * progress;
	float angle = static_cast<float>(_mShakeTimer->GetTime()) * 50.0f;
	float shakeX = intensity * cosf(angle);
	float shakeY = intensity * sinf(angle);

	// オフセットに揺れを代入
	mOffset.x = shakeX;
	mOffset.y = shakeY;

	// 現在時間が0なら揺れの強さとオフセットを0にする
	if (_mShakeTimer->IsTimeUp()) {
		mShakePower = 0.0f;
		mOffset = { 0.0f, 0.0f, 0.0f };
	}

	_mShakeTimer->Update(deltaTime);
}

void Shaker::Shake(float power, double shakeTime)
{
	// 揺れの強さをセット
	mShakePower = power;

	// タイマーをセット
	_mShakeTimer->Start(shakeTime);
}

Vector3 Shaker::GetShakeOffset()
{
	return mOffset;
}

bool Shaker::IsSeeking()
{
	return _mShakeTimer->GetEnable();
}
