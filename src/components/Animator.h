/*============================================================
*	@file	 : Animator.h
*	@brief	 : アニメーションコンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/07
*	@updated : 2026/10/05
*============================================================*/
#pragma once

#include "Component.h"
#include "Animation.h"
#include <string>
#include <unordered_map>

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
    // 再生中アニメーション名
    std::string mAnimKey{};

    // スケルトン
    Skeleton* _mSkeleton{};

    // アニメーション
    Animation* _mAnimation{};
    
    // アニメーション経過時間
    double mCurrentTime{};

    // ノードテーブル
    std::unordered_map<std::string, std::vector<int>> mNodeTable{};

public:
    Animator(GameObject* owner);

    void Finalize() override;

    // アニメーション読み込み
    Animator* Load(std::string keyName, const char* fileName);

    // アニメーションをセット
    void Set(const std::string& keyName);

    // 更新
    void Update(double deltaTime) override;

    // アニメーション名取得
    std::string GetAnimKey() const { return mAnimKey; }

    // 経過時間取得
    double GetTime() const { return mCurrentTime; }

private:
    // モデルのスケルトンをセット
    bool setSkeleton();

    // ノード対応を解決
    void generateNodeTable(const std::string& keyName);

    // ボーンアニメーション計算
    void calculateBoneTransform(const Animation::CHANNEL& channel, int nodeIndex, double time);
    Vector3 calculatePosition(const std::vector<Animation::KEY_POSITION>& keys, double time);
    Quaternion calculateRotation(const std::vector<Animation::KEY_ROTATION>& keys, double time);
    Vector3 calculateScale(const std::vector<Animation::KEY_SCALE>& keys, double time);
};