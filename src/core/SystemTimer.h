/*============================================================
*	@file	 : SystemTimer.h
*	@brief	 : ゲーム内時間制御
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/04/13
*	@updated : 2026/11/09
*============================================================*/
#pragma once

#include <Windows.h>
#include <cmath>

namespace System {
/*============================================================
*	@class	: System::Timer
*	@brief	: システムタイマー
*============================================================*/
	class Timer final
	{
	/*--------------------------------------------------
		Singleton用
	----------------------------------------------------*/
	public:
		static Timer& getInstance() {
			static Timer instance;
			return instance;
		}

	private:
		Timer() = default;
		Timer(const Timer&) = delete;

		Timer& operator=(const Timer&) = delete;
		Timer(Timer&&) = delete;

		Timer& operator=(Timer&&) = delete;
		~Timer() {};

	/*--------------------------------------------------
		メンバ変数・メンバ関数
	----------------------------------------------------*/
	private:
		LARGE_INTEGER mFreq{};
		LARGE_INTEGER mExecLastTime{};
		LARGE_INTEGER mCurrentTime{};
		double mAccumulator{};
		double mFps{ 1.0 / 60.0 };

	public:
		void Initialize();

		// タイマー進行
		bool Tick();

		// タイマーリセット
		void Refresh();

		// 現在時間取得
		double GetCurrent() const { return static_cast<double>(mCurrentTime.QuadPart); }

		// 累積時間取得
		double GetAccumulator() const { return mAccumulator; }

		// dt取得
		double GetDeltaTime() const { return mFps; }

		// フレームレート取得
		int GetFps() const { return static_cast<int>(std::round(1.0 / mFps)); }

		// FPS値変更
		void SetFps(double fps) { mFps = 1.0 / fps; }
	};
}