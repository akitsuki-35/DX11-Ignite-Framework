/*============================================================
*	@file	 : Transform.h
*	@brief	 : トランスフォーム
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/05/19
*	@updated : 2026/11/08
*============================================================*/
#pragma once

#include "Vector3.h"
#include <DirectXMath.h>

/*============================================================
*	@class	: Transform
*	@brief	: トランスフォーム
*============================================================*/
class Transform
{
	friend class ParticleRenderer;

private:
	Vector3 mPosition{ 0.0f, 0.0f, 0.0f };
	Vector3 mRotation{ 0.0f, 0.0f, 0.0f };
	Vector3 mScale{ 1.0f, 1.0f, 1.0f };

	// 更新フラグ
	mutable bool mDirty{ true };

	// ワールド行列
	mutable DirectX::XMMATRIX mWorldMatrix{};

public:
	Transform() = default;
	Transform(Vector3 position, Vector3 rotation = { 0.0f, 0.0f, 0.0f }, Vector3 scale = { 1.0f, 1.0f, 1.0f })
		: mPosition(position), mRotation(rotation), mScale(scale) {}

private:
	// 行列作成
	DirectX::XMMATRIX createScaleMatrix() const;
	DirectX::XMMATRIX createRotationMatrix() const;
	DirectX::XMMATRIX createTranslationMatrix() const;

	// ビルボード用回転行列作成
	DirectX::XMMATRIX createBillboardRotation(const DirectX::XMMATRIX& view) const;

	// ワールド行列を更新
	// ビルボード対応用に引数でRotationを生成
	void rebuildWorldMatrix(const DirectX::XMMATRIX& rotation) const;

	// 更新フラグをオン
	void markDirty() { mDirty = true; }

public:
	// セッター
	// メンバ更新後、更新フラグ有効化
	void SetPosition(const Vector3& position) {
		mPosition = position;
		markDirty();
	}
	void SetRotation(const Vector3& rotation) {
		mRotation = rotation;
		markDirty();
	}
	void SetScale(const Vector3& scale) {
		mScale = scale;
		markDirty();
	}

	// ワールド行列をセット
	void SetWorldMatrix(const DirectX::XMMATRIX& world)
	{
		mWorldMatrix = world;
		mDirty = false;
	}

	// ゲッター
	const Vector3& GetPosition() const { return mPosition; }
	const Vector3& GetRotation() const { return mRotation; }
	const Vector3& GetScale() const { return mScale; }

	// ワールド行列取得
	DirectX::XMMATRIX GetWorldMatrix() const;

	// ビルボード用ワールド行列取得
	DirectX::XMMATRIX GetBillboardMatrix(const DirectX::XMMATRIX& view) const;

	// 前方向取得
	Vector3 GetForward() const;

	// 右方向取得
	Vector3 GetRight() const;

	// 上方向取得
	Vector3 GetUp() const;
};