/*============================================================
*	@file	 : Collider.cpp
*	@brief	 : コライダーコンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
*	@updated : 2026/11/11
* 　@date	 : 2026/11/11
*============================================================*/
#include "Collider.h"
#include "GameObject.h"

bool Collider::IsHit(const Collider& other) const
{
    // 自身と相手の中心位置をワールド座標で取得
    const Vector3 thisCenter = getWorldCenter();
    const Vector3 otherCenter = other.getWorldCenter();

    // 自身と相手のサイズの半分を取得（中心～端のサイズにする）
    const Vector3 thisHalfSize = mSize * 0.5f;
    const Vector3 otherHalfSize = other.mSize * 0.5f;

    // 自身と相手のAABBの最小座標と最大座標を算出
    const Vector3 thisMin = thisCenter - thisHalfSize;
    const Vector3 thisMax = thisCenter + thisHalfSize;

    const Vector3 otherMin = otherCenter - otherHalfSize;
    const Vector3 otherMax = otherCenter + otherHalfSize;

    // 3軸の範囲が重なっていれば衝突
    return thisMin.x <= otherMax.x && thisMax.x >= otherMin.x &&
        thisMin.y <= otherMax.y && thisMax.y >= otherMin.y && 
        thisMin.z <= otherMax.z && thisMax.z >= otherMin.z;
}

Collider* Collider::SetCenter(const Vector3& center)
{
    mCenter = center;
    return this;
}

Collider* Collider::SetSize(const Vector3& size)
{
    mSize = size;
    return this;
}

Vector3 Collider::getWorldCenter() const
{
    // オブジェクトのワールド座標にコライダーのローカル座標を加算
    return _mOwner->GetPosition() + mCenter;
}