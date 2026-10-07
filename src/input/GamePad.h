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
#include <vector>
#include <Xinput.h>
#pragma comment(lib, "xinput.lib")

/*------------------------------------------------------------
	ゲームパッド名前解決テーブル
------------------------------------------------------------*/
enum class ButtonCode : WORD
{
	None = 0x00,

	A = XINPUT_GAMEPAD_A,
	B = XINPUT_GAMEPAD_B,
	X = XINPUT_GAMEPAD_X,
	Y = XINPUT_GAMEPAD_Y,
	Left = XINPUT_GAMEPAD_DPAD_LEFT,
	Right = XINPUT_GAMEPAD_DPAD_RIGHT,
	Up = XINPUT_GAMEPAD_DPAD_UP,
	Down = XINPUT_GAMEPAD_DPAD_DOWN,
	L = XINPUT_GAMEPAD_LEFT_SHOULDER,
	R = XINPUT_GAMEPAD_RIGHT_SHOULDER,
	Start = XINPUT_GAMEPAD_START,
	Select = XINPUT_GAMEPAD_BACK,
	StickL = XINPUT_GAMEPAD_LEFT_THUMB,
	StickR = XINPUT_GAMEPAD_RIGHT_THUMB
};

/*------------------------------------------------------------
	ボタン構造体
------------------------------------------------------------*/
struct BUTTON
{
	std::vector<ButtonCode> buttons{};

	BUTTON() = default;
	BUTTON(ButtonCode button) { if (button != ButtonCode::None) buttons.push_back(button); }
};

/*------------------------------------------------------------
	複数ボタン対応用オペレーター
------------------------------------------------------------*/
inline BUTTON operator||(ButtonCode a, ButtonCode b)
{
	BUTTON g{};
	g.buttons.push_back(a);
	g.buttons.push_back(b);
	return g;
}

inline BUTTON operator||(BUTTON g, ButtonCode b)
{
	g.buttons.push_back(b);
	return g;
}


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
	static bool IsPressed(BUTTON button, int index = 0);
	static bool IsTriggered(BUTTON button, int index = 0);
	static bool IsReleaseed(BUTTON button, int index = 0);

	// アナログスティック入力
	static float GetLeftAxisX(int index = 0);
	static float GetLeftAxisY(int index = 0);
	static float GetRightAxisX(int index = 0);
	static float GetRightAxisY(int index = 0);

	// トリガー入力
	static float GetLeftTrigger(int index = 0);
	static float GetRightTrigger(int index = 0);
};