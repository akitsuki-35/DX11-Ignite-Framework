/*============================================================
*	@file	 : Camera.h
*	@brief	 : カメラ基底クラス
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/04/26
*	@updated : 2026/10/09
*============================================================*/
#pragma once

#include "GameObject.h"
#include <DirectXMath.h>

/*============================================================
*	@class	: Camera
*	@brief	: カメラ基底クラス
*============================================================*/
class Camera : public GameObject
{
protected:
	// 注視点
	Vector3 mTarget{};

	// ビュー行列
	DirectX::XMMATRIX mViewMatrix{};

public:
	virtual ~Camera() = default;
	virtual void Initialize() override;
	virtual void Finalize() override;
	virtual void Update(double deltaTime) override;

	// ビュー行列取得
	DirectX::XMMATRIX GetViewMatrix() const { return mViewMatrix; }

	// カメラ前方・後方を取得
	// ゲームオブジェクトのTransform取得とは別扱い
	Vector3 GetForward() const;
	Vector3 GetRight() const;

	// ビュー行列のセット
	void SetMatrix() const;
};