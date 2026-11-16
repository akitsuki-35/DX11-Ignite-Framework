/*============================================================
*	@file	 : SystemTimer.cpp
*	@brief	 : ゲーム内時間制御
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/04/13
*	@updated : 2026/11/16
*============================================================*/
#include "SystemTimer.h"

void System::Timer::Initialize()
{
    // タイマー初期化
    QueryPerformanceFrequency(&mFreq);
    QueryPerformanceCounter(&mExecLastTime);
}

bool System::Timer::Tick()
{
    // タイマー進行
    QueryPerformanceCounter(&mCurrentTime);

    // dt更新
    double dt = static_cast<double>(mCurrentTime.QuadPart - mExecLastTime.QuadPart)
        / static_cast<double>(mFreq.QuadPart);

    mExecLastTime = mCurrentTime;

    // 累積時間更新
    mAccumulator += dt;

#ifndef NDEBUG
    // FPS計測時間を加算
    mFpsTimer += dt;
#endif

    if (mAccumulator >= mFrameRate) {

#ifndef NDEBUG
        // FPS計測
        measureFps();
#endif

        mAccumulator -= mFrameRate;
        return true;
    }

    return false;
}

void System::Timer::Refresh()
{
    // 累積時間を現在時間でリセット
    QueryPerformanceCounter(&mCurrentTime);

    mExecLastTime = mCurrentTime;

    mAccumulator = 0.0;
}

#ifndef NDEBUG
void System::Timer::measureFps()
{
    // FPS計測
    mFrameCount++;

    if (mFpsTimer >= 1.0) {
        mFps = static_cast<double>(mFrameCount) / mFpsTimer;
        mFpsTimer = 0.0;
        mFrameCount = 0;
    }
}
#endif