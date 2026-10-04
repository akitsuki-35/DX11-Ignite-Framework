/*============================================================
*	@file	 : Animator.h
*	@brief	 : アニメーションコンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/07
*	@updated : 2026/10/01
*============================================================*/
#pragma once

#include "Component.h"
#include "Animation.h"
#include <string>
#include <cassert>

/*------------------------------------------------------------
	前方宣言
------------------------------------------------------------*/
class Skeleton;
class Animation;

/*============================================================
*	@class	: Animator
*	@brief	: アニメーションコンポーネント
*============================================================*/
class Animator final : public Component
{
private:
    // スケルトン
    Skeleton* _mSkeleton{};

    // アニメーション
    Animation* _mAnimation{};
    
    // アニメーション経過時間
    double mCurrentTime{};

public:
    Animator(GameObject* owner)
        : Component(owner) {}

    void Finalize() override {
        _mSkeleton = nullptr;
        _mAnimation = nullptr;
    }

    // アニメーションをセット
    void Set(const std::string& keyName);

    // 更新
    void Update(double deltaTime) override;

    // 経過時間取得
    double GetTime() const { return mCurrentTime; }

private:
    // モデルのスケルトンをセット
    bool setSkeleton();

    // ボーンアニメーション計算
    void calculateBoneTransform(const Animation::CHANNEL& channel, double time);
    Vector3 calculatePosition(const std::vector<Animation::KEY_POSITION>& keys, double time);
    Quaternion calculateRotation(const std::vector<Animation::KEY_ROTATION>& keys, double time);
    Vector3 calculateScale(const std::vector<Animation::KEY_SCALE>& keys, double time);
};