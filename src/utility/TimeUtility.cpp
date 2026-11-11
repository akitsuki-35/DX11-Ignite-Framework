/*============================================================
*	@file	 : TimeUtility.cpp
*	@brief	 : タイム関連ユーティリティ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/09
*	@updated : 2026/11/10
*============================================================*/
#include "TimeUtility.h"
#include "SystemTimer.h"

// システムタイマーからフレームレート取得
namespace {
    int getFps() {
        return System::Timer::getInstance().GetFps();
    }
}

double Utility::Time::toSeconds(int frame)
{
    int fps = getFps();

    // フレーム→秒に変換
    double seconds = static_cast<double>(frame / fps);

    return seconds;
}

int Utility::Time::toFrame(double seconds)
{
    int fps = getFps();

    // 秒→フレームに変換
    int frame = static_cast<int>(seconds * fps);

    return frame;
}
