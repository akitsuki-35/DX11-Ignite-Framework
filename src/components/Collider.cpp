/*============================================================
*	@file	 : Collider.cpp
*	@brief	 : コライダーコンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
*	@updated : 2026/11/11
* 　@date	 : 2026/11/25
*============================================================*/
#include "Collider.h"
#include "Animator.h"
#include "GameObject.h"
#include <cmath>

#ifndef NDEBUG
#include "DebugRenderer.h"
#endif

Collider::Collider(GameObject* owner)
    : Component(owner)
{
#ifndef NDEBUG
    _mDebug = _mOwner->AddComponent<DebugRenderer>(_mOwner);
    _mDebug->CreateBox(mCenter, mSize);
#endif
}

void Collider::Update(double /*deltaTime*/)
{
    // Animatorコンポーネント取得
    if (!_mAnimator) {
        _mAnimator = _mOwner->GetComponent<Animator>();
    }

    if (_mDebug) {
        _mDebug->SetOffset(getWorldCenter() - _mOwner->GetPosition() - mCenter);
    }
}

bool Collider::IsHit(const Collider& other) const
{
    if (!mEnable) return false;

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

Vector3 Collider::GetPushBack(const Collider& other) const
{
    if (!mEnable) return Vector3{};

    const Vector3 thisCenter = getWorldCenter();
    const Vector3 otherCenter = other.getWorldCenter();

    const Vector3 thisHalfSize = mSize * 0.5f;
    const Vector3 otherHalfSize = other.mSize * 0.5f;

    // 各軸の中心間距離
    const Vector3 diff = thisCenter - otherCenter;

    // 各軸でどれだけ重なっているか
    const float overlapX = (thisHalfSize.x + otherHalfSize.x) - std::abs(diff.x);
    const float overlapY = (thisHalfSize.y + otherHalfSize.y) - std::abs(diff.y);
    const float overlapZ = (thisHalfSize.z + otherHalfSize.z) - std::abs(diff.z);

    // 重なっていなければ押し出さない
    if (overlapX <= 0.0f || overlapY <= 0.0f || overlapZ <= 0.0f) {
        return {};
    }

    Vector3 pushBack{};

    // 一番重なりが小さい軸から押し出す
    if (overlapX <= overlapY && overlapX <= overlapZ) {
        pushBack.x = (diff.x < 0.0f) ? -overlapX : overlapX;
    }
    else if (overlapY <= overlapX && overlapY <= overlapZ) {
        pushBack.y = (diff.y < 0.0f) ? -overlapY : overlapY;
    }
    else {
        pushBack.z = (diff.z < 0.0f) ? -overlapZ : overlapZ;
    }

    return pushBack;
}

Collider* Collider::SetCenter(const Vector3& center)
{
    mCenter = center;

#ifndef NDEBUG
    createDebugBox();
#endif

    return this;
}

Collider* Collider::SetSize(const Vector3& size)
{
    mSize = size;

#ifndef NDEBUG
    createDebugBox();
#endif

    return this;
}

Collider* Collider::SetEnable(const bool& enable)
{
    mEnable = enable;
    return this;
}

Collider* Collider::SetRootNode(const std::string& name)
{
    mRootNode = name;
    return this;
}

Vector3 Collider::getWorldCenter() const
{
    // 追従ノードなし
    if (!_mAnimator || mRootNode.empty()) {
        return _mOwner->GetPosition() + mCenter;
    }

    // 追従ノードあり
    const DirectX::XMFLOAT4X4* nodeMatrix =  _mAnimator->GetNodeGlobalMatrix(mRootNode);

    // ノードが存在しない
    if (!nodeMatrix) {
        return _mOwner->GetPosition() + mCenter;
    }

    // ノードのモデル空間上の位置
    DirectX::XMVECTOR nodePos = DirectX::XMVectorSet(nodeMatrix->_41, nodeMatrix->_42, 
        nodeMatrix->_43, 1.0f);

    // +オブジェクトのワールド行列でワールド座標に変換
    nodePos = DirectX::XMVector3TransformCoord(nodePos, _mOwner->GetTransform().GetWorldMatrix());

    DirectX::XMFLOAT3 worldPos{};
    DirectX::XMStoreFloat3(&worldPos, nodePos);

    // 追従ノードからのオフセット
    return Vector3{worldPos.x, worldPos.y, worldPos.z} + mCenter;
}

/*------------------------------------------------------------
    デバッグ用描画関連
------------------------------------------------------------*/
#ifndef NDEBUG
Collider* Collider::LoadShader(const std::string& keyname)
{
    if (_mDebug) {
        _mDebug->LoadShader(keyname);
    }

    return this;
}

Collider* Collider::SetColor(const DirectX::XMFLOAT4& color)
{
    _mDebug->SetColor(color);
    return this;
}

void Collider::createDebugBox()
{
    if (!_mDebug) {
        return;
    }

    _mDebug->Clear();
    _mDebug->CreateBox(mCenter, mSize);
}

#endif