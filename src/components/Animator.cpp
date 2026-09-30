/*============================================================
*	@file	 : Animator.cpp
*	@brief	 : アニメーションコンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/07
*	@updated : 2026/09/30
*============================================================*/
#include "Animator.h"
#include "Model.h"
#include "BoneTransform.h"
#include "AnimationManager.h"
#include "ModelRenderer.h"
#include "GameObject.h"
#include <cmath>
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

    // Tickへ変換
    mCurrentTime += deltaTime * _mAnimation->GetTicksPerSecond();

    // アニメーションループ
    if (mCurrentTime >= _mAnimation->GetDuration()) {
        mCurrentTime = std::fmod(mCurrentTime, _mAnimation->GetDuration());
    }

    // チャンネル更新
    for (auto& channel : _mAnimation->GetChannels()) {
        calculateBoneTransform(channel, mCurrentTime);
    }

    _mSkeleton->Update();
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
    BoneTransform transform{};

    // 座標更新
    transform.mPosition = calculatePosition(channel.Positions, time);

    // 回転更新
    transform.mRotation = calculateRotation(channel.Rotations, time);

    // スケール更新
    transform.mScale = calculateScale(channel.Scales, time);

    auto& bone = _mSkeleton->GetBone(channel.BoneIndex);

    XMMATRIX animLocal = transform.ToMatrix();
    XMMATRIX finalLocal = animLocal;

    XMStoreFloat4x4(&bone.Local, finalLocal);
}

Vector3 Animator::calculatePosition(const std::vector<Animation::KEY_POSITION>& keys, double time)
{
    if (keys.empty()) {
        return { 0.0f, 0.0f, 0.0f };
    }

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
    if (keys.empty()) {
        return { 0.0f, 0.0f, 0.0f, 1.0f };
    }

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
    if (keys.empty()) {
        return { 1.0f, 1.0f, 1.0f };
    }

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
