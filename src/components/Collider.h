/*============================================================
*	@file	 : Collider.h
*	@brief	 : コライダーコンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
*	@updated : 2026/11/11
* 　@date	 : 2026/11/25
*============================================================*/
#pragma once

#include "Component.h"
#include "Vector3.h"
#include <string>

// デバッグでしか使わない
#ifndef NDEBUG
#include <DirectXMath.h>
#endif

/*------------------------------------------------------------
	前方宣言
------------------------------------------------------------*/
class Animator;
#ifndef NDEBUG
class DebugRenderer;
#endif

/*============================================================
*	@class	: Collider
*	@brief	: コライダーコンポーネント
*============================================================*/
class Collider : public Component
{
private:
    // 当たり判定の中心（ゲームオブジェクト中心座標からのオフセット）
    Vector3 mCenter{};

    // コライダーのサイズ
    Vector3 mSize{ 1.0f, 1.0f, 1.0f };

    // コライダー有効状態
    bool mEnable{ true };

    // 追従対象
    Animator* _mAnimator{ nullptr };
    std::string mRootNode{};

#ifndef NDEBUG
    // デバッグ表示
    DebugRenderer* _mDebug{ nullptr };
#endif

public:
    Collider(GameObject* owner);

    void Update(double deltaTime) override;

    // 衝突判定
    bool IsHit(const Collider& other) const;

    // 衝突した相手から押し出す量を取得
    Vector3 GetPushBack(const Collider& other) const;

    // 中心位置のセット・取得
    Collider* SetCenter(const Vector3& center);
    const Vector3& GetCenter() const { return mCenter; }

    // サイズのセット・取得
    Collider* SetSize(const Vector3& size);
    const Vector3& GetSize() const { return mSize; }

    // 有効状態のセット・取得
    Collider* SetEnable(const bool& enable);
    const bool& GetEnable() const { return mEnable; }

    // 追従ノードをセット
    Collider* SetRootNode(const std::string& name);

private:
    // ワールド座標上の中心座標取得
    Vector3 getWorldCenter() const;


#ifndef NDEBUG
public:
    // シェーダー読み込み
    Collider* LoadShader(const std::string& keyname);

    // ライン色変更
    Collider* SetColor(const DirectX::XMFLOAT4& color);

private:
    void createDebugBox();
#endif
};