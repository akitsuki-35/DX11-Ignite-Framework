/*============================================================
*	@file	 : Animator.cpp
*	@brief	 : アニメーションコンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/07
*	@updated : 2026/10/09
*============================================================*/
#include "Animator.h"
#include "Model.h"
#include "AnimationManager.h"
#include "ModelRenderer.h"
#include "GameObject.h"
#include <cmath>
#include <algorithm>

using namespace DirectX;

Animator::Animator(GameObject* owner)
    : Component(owner)
{
    // モデルのスケルトン取得
    setSkeleton();
}

void Animator::Finalize()
{
    mNext._Animation = nullptr;
    mCurrent._Animation = nullptr;
}

void Animator::Update(double deltaTime)
{
    if (!mCurrent._Animation || !mNext._Animation) {
        return;
    }

    // Current != Nextの場合はブレンド処理
    if (mCurrent.Name != mNext.Name) {
        updateBlend(deltaTime);
    }
    else {
        updateCurrent(deltaTime);
    }
}

Animator* Animator::Load(std::string keyName, const char* fileName, const bool& isSet)
{
    Animation* animation = AnimationManager::getInstance().Load(keyName, fileName);
    
    // ノードテーブル作成
    generateNodeTable(keyName, animation);

    // ロードしたアニメーションをセット
    if (isSet) {
        Set(keyName);
    }

    return this;
}

void Animator::Set(const std::string& keyName, const bool& isLoop, const double& duration)
{
    if (mNext.Name == keyName) {
        return;
    }

    Animation* animation = AnimationManager::getInstance().Get(keyName);

    if (!animation) {
        return;
    }

    if (!mNodeTable.contains(keyName)) {
        generateNodeTable(keyName, animation);
    }

    mNext._Animation = animation;
    mNext.Name = keyName;
    mNext.ElapsedTime = 0.0;
    mNext.IsLoop = isLoop;

    // Currentが存在する場合はアニメーションブレンド処理
    if (!mCurrent._Animation) {
        mCurrent = mNext;
    }
    else {
        animBlend(duration);
    }
}

bool Animator::IsPlaying(std::string keyName) const
{
    if (mCurrent.Name == keyName || mNext.Name == keyName) {
        if (!IsFinished(keyName)) {
            return true;
        }
    }

    return false;
}

bool Animator::IsFinished(std::string keyName) const
{
    if (mCurrent.Name != keyName && mNext.Name != keyName) {
        return false;
    }

    return mCurrent.ElapsedTime >= mCurrent._Animation->GetDuration();
}

bool Animator::IsBlending(std::string keyName) const
{
    if (mNext.Name != keyName) {
        return false;
    }

    return mCurrent.Name != mNext.Name;
}

void Animator::updateCurrent(double deltaTime)
{
    // 全ノードをバインドポーズに戻す
    mSkeleton.ToBindPose();

    // ノードテーブル参照でアニメーション更新
    const auto& channels = mCurrent._Animation->GetChannels();
    const auto& table = mNodeTable.at(mCurrent.Name);

    for (size_t i = 0; i < channels.size(); ++i) {
        updateBoneTransform(channels[i], table[i], mCurrent.ElapsedTime);
    }

    mSkeleton.Update();

    // 経過時間更新
    mCurrent.ElapsedTime += deltaTime * mCurrent._Animation->GetTicksPerSecond();

    looping(mCurrent);
}

void Animator::updateBlend(double deltaTime)
{
    // 全ノードをバインドポーズに戻す
    mSkeleton.ToBindPose();

    // 現在アニメーションのチャンネルとテーブル取得
    const auto& currentChannels = mCurrent._Animation->GetChannels();
    const auto& currentTable = mNodeTable.at(mCurrent.Name);

    // 次アニメーションのチャンネルとテーブル取得
    const auto& nextChannels = mNext._Animation->GetChannels();
    const auto& nextTable = mNodeTable.at(mNext.Name);

    // Next用の逆引きノードテーブル作成
    std::vector<int> nextNodes(mSkeleton.GetNodeCount(), -1);

    for (size_t i = 0; i < nextTable.size(); ++i) {
        int nodeIndex = nextTable[i];

        if (nodeIndex < 0 || static_cast<size_t>(nodeIndex) >= nextNodes.size()) {
            continue;
        }

        nextNodes[nodeIndex] = static_cast<int>(i);
    }

    // ウェイト算出
    float weight = 1.0f;
    if (mBlendDuration > 0.0) {
        weight = std::clamp(static_cast<float>(mBlendTime / mBlendDuration), 0.0f, 1.0f);

        // エルミート補間
        weight = weight * weight * (3.0f - 2.0f * weight);
    }

    // チャンネル更新
    // 逆引きテーブルを元にNextを更新する
    for (size_t i = 0; i < currentChannels.size(); ++i) {
        const int nodeIndex = currentTable[i];

        if (nodeIndex < 0 || static_cast<size_t>(nodeIndex) >= mSkeleton.GetNodeCount()) {
            continue;
        }

        const int nextIndex = nextNodes[nodeIndex];

        if (nextIndex < 0 || static_cast<size_t>(nextIndex) >= nextChannels.size()) {
            updateBoneTransform(currentChannels[i], nodeIndex, mCurrent.ElapsedTime);
            continue;
        }

        // 現在アニメーションのトランスフォーム更新
        BoneTransform current = calculateBoneTransform(currentChannels[i], mCurrent.ElapsedTime);

        // 次アニメーションのトランスフォーム更新
        BoneTransform next = calculateBoneTransform(nextChannels[nextIndex], mNext.ElapsedTime);

        // ブレンド処理
        BoneTransform blend{};
        blend.mPosition = Vector3::Lerp(current.mPosition, next.mPosition, weight);
        blend.mRotation = Quaternion::Slerp(current.mRotation, next.mRotation, weight);
        blend.mScale = Vector3::Lerp(current.mScale, next.mScale, weight);

        auto& node = mSkeleton.GetNode(nodeIndex);
        XMStoreFloat4x4(&node.Local, blend.ToMatrix());
    }

    mSkeleton.Update();

    // 現在アニメーションと次アニメーションの経過時間を更新
    mCurrent.ElapsedTime += deltaTime * mCurrent._Animation->GetTicksPerSecond();
    mNext.ElapsedTime += deltaTime * mNext._Animation->GetTicksPerSecond();

    // 各アニメーションのループ
    looping(mCurrent);
    looping(mNext);

    // ブレンド進行
    mBlendTime += deltaTime;

    // ブレンド完了処理
    if (mBlendTime >= mBlendDuration) {
        // Current == Next
        mCurrent = mNext;

        mBlendTime = 0.0f;
        mBlendDuration = 0.0f;
    }
}

void Animator::looping(ANIMATION& animation)
{
    // ループフラグに準じたループ処理
    double duration = animation._Animation->GetDuration();
    if (duration > 0.0) {
        if (animation.IsLoop) {
            animation.ElapsedTime = std::fmod(animation.ElapsedTime, duration);
        }
        else {
            animation.ElapsedTime = std::min(animation.ElapsedTime, duration);
        }
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
    mSkeleton = model->GetSkeleton();

    return true;
}

void Animator::generateNodeTable(const std::string& keyName, const Animation* animation)
{
    mNodeTable[keyName].clear();

    if (!animation) return;

    const auto& channels = animation->GetChannels();

    // アニメーションチャンネルからノードテーブルを作成
    for (size_t i = 0; i < channels.size(); ++i) {
        mNodeTable[keyName].push_back(mSkeleton.FindNode(channels[i].NodeName));
    }
}

void Animator::updateBoneTransform(const Animation::CHANNEL& channel, int nodeIndex, double time)
{
    if (nodeIndex < 0 || static_cast<size_t>(nodeIndex) >= mSkeleton.GetNodeCount()) {
        return;
    }

    // ノード取得
    auto& node = mSkeleton.GetNode(nodeIndex);

    BoneTransform transform{};

    // トランスフォーム計算
    transform = calculateBoneTransform(channel, time);

    // BindLocalを作らない
    XMStoreFloat4x4(&node.Local, transform.ToMatrix());
}

BoneTransform Animator::calculateBoneTransform(const Animation::CHANNEL& channel, double time)
{
    BoneTransform transform{};

    // 座標計算
    transform.mPosition = calculatePosition(channel.Positions, time);

    // 回転計算
    transform.mRotation = calculateRotation(channel.Rotations, time);

    // スケール計算
    transform.mScale = calculateScale(channel.Scales, time);

    return transform;
}

Vector3 Animator::calculatePosition(const std::vector<Animation::KEY_POSITION>& keys, double time)
{
    if (keys.empty()) {
        return Vector3{};
    }

    // キーが1つのみの場合は補間しない
    if (keys.size() == 1 || time <= keys.front().Time) {
        return keys.front().Position;
    }

    if (time >= keys.back().Time) {
        return keys.back().Position;
    }

    int index = 0;

    for (int i = 0; i < static_cast<int>(keys.size()) - 1; i++) {
        if (time < keys[i + 1].Time) {
            index = i;
            break;
        }
    }

    for (size_t i = 0; i + 1 < keys.size(); ++i) {
        const auto& current = keys[i];
        const auto& next = keys[i + 1];

        if (time <= next.Time) {
            const double interval = next.Time - current.Time;

            if (interval <= 0.0) {
                return current.Position;
            }

            const float factor = std::clamp(static_cast<float>((time - current.Time) / interval), 0.0f, 1.0f);

            return Vector3::Lerp(current.Position, next.Position, factor);
        }
    }

    return keys.back().Position;
}

Quaternion Animator::calculateRotation(const std::vector<Animation::KEY_ROTATION>& keys, double time)
{
    if (keys.empty()) {
        return Quaternion{};
    }

    // キーが1つのみの場合は補間しない
    if (keys.size() == 1 || time <= keys.front().Time) {
        return keys.front().Rotation;
    }

    if (time >= keys.back().Time) {
        return keys.back().Rotation;
    }

    for (size_t i = 0; i + 1 < keys.size(); ++i) {
        const auto& current = keys[i];
        const auto& next = keys[i + 1];

        if (time <= next.Time) {
            const double interval = next.Time - current.Time;

            if (interval <= 0.0) {
                return current.Rotation;
            }

            const float factor = std::clamp(static_cast<float>((time - current.Time) / interval), 0.0f, 1.0f);

            return Quaternion::Slerp(current.Rotation, next.Rotation, factor);
        }
    }

    return keys.back().Rotation;
}

Vector3 Animator::calculateScale(const std::vector<Animation::KEY_SCALE>& keys, double time)
{
    if (keys.empty()) {
        return Vector3{ 1.0f, 1.0f, 1.0f };
    }

    // キーが1つのみの場合は補間しない
    if (keys.size() == 1 || time <= keys.front().Time) {
        return keys.front().Scale;
    }

    if (time >= keys.back().Time) {
        return keys.back().Scale;
    }

    for (size_t i = 0; i + 1 < keys.size(); ++i) {
        const auto& current = keys[i];
        const auto& next = keys[i + 1];

        if (time <= next.Time) {
            const double interval = next.Time - current.Time;

            if (interval <= 0.0) {
                return current.Scale;
            }

            const float factor = std::clamp(static_cast<float>((time - current.Time) / interval), 0.0f, 1.0f);

            return Vector3::Lerp(current.Scale, next.Scale, factor);
        }
    }

    return keys.back().Scale;
}

void Animator::animBlend(const double& duration)
{
    mNext.ElapsedTime = 0.0;

    mBlendTime = 0.0;
    mBlendDuration = duration;
}