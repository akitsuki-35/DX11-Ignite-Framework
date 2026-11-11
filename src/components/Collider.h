/*============================================================
*	@file	 : Collider.h
*	@brief	 : コライダーコンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
*	@updated : 2026/11/11
* 　@date	 : 2026/11/11
*============================================================*/
#pragma once

#include "Component.h"
#include "Vector3.h"

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

public:
    Collider(GameObject* owner) 
        : Component(owner){}

    // 衝突判定
    bool IsHit(const Collider& other) const;

    // 中心位置のセット・取得
    Collider* SetCenter(const Vector3& center);
    const Vector3& GetCenter() const { return mCenter; }

    // サイズのセット・取得
    Collider* SetSize(const Vector3& size);
    const Vector3& GetSize() const { return mSize; }

private:
    // ワールド座標上の中心座標取得
    Vector3 getWorldCenter() const;
};