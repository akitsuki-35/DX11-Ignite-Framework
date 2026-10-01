/*============================================================
*	@file	 : Animator.cpp
*	@brief	 : アニメーションコンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/07
*	@updated : 2026/10/01
*============================================================*/
#include "Animator.h"
#include "Model.h"
#include "BoneTransform.h"
#include "AnimationManager.h"
#include "ModelRenderer.h"
#include "GameObject.h"
#include <cmath>
#include <algorithm>
#include <cassert>

using namespace DirectX;

void Animator::Set(const std::string& keyName)
{
    _mAnimation = AnimationManager::getInstance().Get(keyName);
    assert(setSkeleton());
    mCurrentTime = 0.0;
}

void Animator::Update(double deltaTime)
{
    if (!_mAnimation || !_mSkeleton) {
        return;
    }

    double duration = _mAnimation->GetDuration();

    // 全ノードをバインドポーズに戻す
    _mSkeleton->ToBindPose();

    // チャンネル更新
    for (auto& channel : _mAnimation->GetChannels()) {
        calculateBoneTransform(channel, mCurrentTime);
    }

    _mSkeleton->Update();

    // Tickへ変換
    mCurrentTime += deltaTime * _mAnimation->GetTicksPerSecond();

    // アニメーションループ
    if (duration > 0.0) {
        mCurrentTime = std::fmod(mCurrentTime, duration);
    }
}

bool Animator::setSkeleton()
{
    Model* model{};

    // オブジェクトのModelRendererコンポーネント取得
    ModelRenderer* renderer = _mOwner->GetComponent<ModelRenderer>();

    if (!renderer) {
        return false;
    }

    // ModelRendererのモデル取得
    model = renderer->GetModel();

    if (!model) {
        return false;
    }

    // モデルからスケルトンを取得
    _mSkeleton = &model->GetSkeleton();

    return true;
}

void Animator::calculateBoneTransform(const Animation::CHANNEL& channel, double time)
{
    if (channel.NodeIndex < 0 || static_cast<size_t>(channel.NodeIndex) >= _mSkeleton->GetNodeCount()) {
        return;
    }

    // ノード取得
    auto& node = _mSkeleton->GetNode(channel.NodeIndex);

    BoneTransform transform{};

    // 座標更新
    transform.mPosition = calculatePosition(channel.Positions, time);

    // 回転更新
    transform.mRotation = calculateRotation(channel.Rotations, time);

    // スケール更新
    transform.mScale = calculateScale(channel.Scales, time);

    // BindLocalを作らない
    XMStoreFloat4x4(&node.Local, transform.ToMatrix());
}

Vector3 Animator::calculatePosition(const std::vector<Animation::KEY_POSITION>& keys, double time)
{
    assert(!keys.empty());

    // キーが1つのみの場合は補間しない
    if (keys.size() == 1) {
        return keys[0].Position;
    }

    int index = 0;

    for (int i = 0; i < static_cast<int>(keys.size()) - 1; i++) {
        if (time < keys[i + 1].Time) {
            index = i;
            break;
        }
    }

    const auto& current = keys[index];
    const auto& next = keys[index + 1];

    float factor = static_cast<float>((time - current.Time) / (next.Time - current.Time));

    return Vector3::Lerp(current.Position, next.Position, factor);
}

Quaternion Animator::calculateRotation(const std::vector<Animation::KEY_ROTATION>& keys, double time)
{
    assert(!keys.empty());

    // キーが1つのみの場合は補間しない
    if (keys.size() == 1) {
        return keys[0].Rotation;
    }

    int index = 0;

    for (int i = 0; i < static_cast<int>(keys.size()) - 1; i++) {
        if (time < keys[i + 1].Time) {
            index = i;
            break;
        }
    }

    const auto& current = keys[index];
    const auto& next = keys[index + 1];

    float factor = static_cast<float>((time - current.Time) / (next.Time - current.Time));

    return Quaternion::Slerp(current.Rotation, next.Rotation, factor);
}

Vector3 Animator::calculateScale(const std::vector<Animation::KEY_SCALE>& keys, double time)
{
    assert(!keys.empty());

    // キーが1つのみの場合は補間しない
    if (keys.size() == 1) {
        return keys[0].Scale;
    }

    int index = 0;

    for (int i = 0; i < static_cast<int>(keys.size()) - 1; i++) {
        if (time < keys[i + 1].Time) {
            index = i;
            break;
        }
    }

    const auto& current = keys[index];
    const auto& next = keys[index + 1];

    float factor = static_cast<float>((time - current.Time) / (next.Time - current.Time));

    return Vector3::Lerp(current.Scale, next.Scale, factor);
}
