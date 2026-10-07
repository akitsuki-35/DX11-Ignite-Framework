/*============================================================
*	@file	 : GamePad.h
*	@brief	 : ゲームパッド入力
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/10/07
*	@updated : 2026/10/07
*============================================================*/
#pragma once

#include <windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput.lib")

/*============================================================
*	@class	: GamePad
*	@brief	: ゲームパッド入力
*============================================================*/
class GamePad final
{
	// ゲームパッドの状態構造体
	struct PADSTATE
	{
		XINPUT_STATE OldPadState{};
		XINPUT_STATE PadState{};
		bool Connected{};
	};

private:
	// 最大4台分の状態を保持
	static constexpr int MAX_CONNECTIONS{ 4 };
	static inline PADSTATE mGamePads[MAX_CONNECTIONS]{};

private:
	GamePad() = delete;

public:
	static void Initialize();
	static void Update();

	// 接続されている？
	static bool IsConnected(int index = 0);

	// ボタン入力
	static bool GetButtonPress(WORD button, int index = 0);
	static bool GetButtonTrigger(WORD button, int index = 0);
	static bool GetButtonRelease(WORD button, int index = 0);

	// アナログスティック入力
	static float GetLeftStickX(int index = 0);
	static float GetLeftStickY(int index = 0);
	static float GetRightStickX(int index = 0);
	static float GetRightStickY(int index = 0);

	// トリガー入力
	static float GetLeftTrigger(int index = 0);
	static float GetRightTrigger(int index = 0);
};