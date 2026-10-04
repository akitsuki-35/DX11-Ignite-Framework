/*============================================================
*	@file	 : Animation.h
*	@brief	 : アニメーションクリップ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/07
*	@updated : 2026/10/01
*============================================================*/
#pragma once

#include "Vector3.h"
#include "Quaternion.h"
#include <string>
#include <vector>
#include <DirectXMath.h>

/*============================================================
*	@class	: Animation
*	@brief	: アニメーションクリップ
*============================================================*/
class Animation final
{
    friend class AssimpLoader;

public:
    // 移動キー
    struct KEY_POSITION
    {
        Vector3 Position{};
        double Time{};
    };

    // 回転キー
    struct KEY_ROTATION
    {
        Quaternion Rotation{};
        double Time{};
    };

    // 拡大縮小キー
    struct KEY_SCALE
    {
        Vector3 Scale{};
        double Time{};
    };

    // アニメーションチャンネル
    struct CHANNEL
    {
        // ノード情報
        std::string NodeName{};
        int NodeIndex{ -1 };

        std::vector<KEY_POSITION> Positions{};
        std::vector<KEY_ROTATION> Rotations{};
        std::vector<KEY_SCALE> Scales{};
    };

private:
    // 総再生時間
    double mDuration{};

    // 1秒あたりのTick数
    double mTicksPerSecond{};

    // チャンネル
    std::vector<CHANNEL> mChannels{};

public:
    // 新規チャンネル追加
    void AddChannel(const CHANNEL& channel);
    
    // ゲッター
    double GetDuration() const { return mDuration; }
    double GetTicksPerSecond() const { return mTicksPerSecond; }
    const std::vector<CHANNEL>& GetChannels() const { return mChannels; }
};