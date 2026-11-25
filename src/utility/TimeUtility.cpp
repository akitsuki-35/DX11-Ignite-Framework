/*============================================================
*	@file	 : TimeUtility.cpp
*	@brief	 : タイム関連ユーティリティ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/09
*	@updated : 2026/11/18
*============================================================*/
#include "TimeUtility.h"
#include "SystemTimer.h"

// システムタイマーからフレームレート取得
namespace {
    int getFrameRate() {
        return System::Timer::getInstance().GetFrameRate();
    }
}

double Utility::Time::toSeconds(int frame)
{
    int frameRate = getFrameRate();

    // フレーム→秒に変換
    double seconds = static_cast<double>(frame) / frameRate;

    return seconds;
}

int Utility::Time::toFrame(double seconds)
{
    int frameRate = getFrameRate();

    // 秒→フレームに変換
    int frame = static_cast<int>(seconds * frameRate);

    return frame;
}
