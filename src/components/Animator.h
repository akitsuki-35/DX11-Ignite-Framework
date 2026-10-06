/*============================================================
*	@file	 : Animator.h
*	@brief	 : アニメーションコンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/07
*	@updated : 2026/10/06
*============================================================*/
#pragma once

#include "Component.h"
#include "Animation.h"
#include "BoneTransform.h"
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
    // アニメーション構造体
    struct ANIMATION
    {
        std::string Name{};
        Animation* _Animation{};
        double ElapsedTime{};
        bool IsLoop{ true };
    };

private:
    // アニメーション
    ANIMATION mCurrent{};
    ANIMATION mNext{};

    // スケルトン
    Skeleton* _mSkeleton{};

    // ブレンド関連
    float mBlendWeight{};
    double mBlendTime{};
    double mBlendDuration{};

    // ノードテーブル
    std::unordered_map<std::string, std::vector<int>> mNodeTable{};

public:
    Animator(GameObject* owner);

    void Finalize() override;

    // アニメーション読み込み
    Animator* Load(std::string keyName, const char* fileName, const bool& isSet = false);

    // アニメーションをセット
    void Set(const std::string& keyName, const bool& isLoop = true, const double& duration = 0.25);

    // 更新
    void Update(double deltaTime) override;

    // 経過時間取得
    double GetTime() const { return mCurrent.ElapsedTime; }

    // 指定アニメーションが再生中か？
    bool IsPlaying(std::string keyName) const;

    // アニメーション再生が完了したか？
    bool IsFinished(std::string keyName) const;

    // ブレンド中？
    bool IsBlending(std::string keyName) const;

private:
    // 単一アニメーションの更新
    void updateCurrent(double deltaTime);

    // ブレンドありの更新
    void updateBlend(double deltaTime);

    // 共通のアニメーションループ処理
    void looping(ANIMATION& animation);

    // モデルのスケルトンをセット
    bool setSkeleton();

    // ノード対応を解決
    void generateNodeTable(const std::string& keyName, const Animation* animation);

    // ボーン更新
    void updateBoneTransform(const Animation::CHANNEL& channel, int nodeIndex, double time);

    // ボーンアニメーション計算
    BoneTransform calculateBoneTransform(const Animation::CHANNEL& channel, double time);
    Vector3 calculatePosition(const std::vector<Animation::KEY_POSITION>& keys, double time);
    Quaternion calculateRotation(const std::vector<Animation::KEY_ROTATION>& keys, double time);
    Vector3 calculateScale(const std::vector<Animation::KEY_SCALE>& keys, double time);

    // アニメーションブレンド
    void animBlend(const double& duration);
};