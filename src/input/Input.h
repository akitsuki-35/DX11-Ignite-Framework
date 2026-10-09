/*============================================================
*	@file	 : Input.h
*	@brief	 : 共通キーバインド・入力
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/10/07
*	@updated : 2026/10/07
*============================================================*/
#pragma once

#include "Keyboard.h"
#include "GamePad.h"
#include <map>

/*------------------------------------------------------------
	ボタン列挙体
------------------------------------------------------------*/
enum class InputKey : int
{
	A,
	B,
	X,
	Y,
	L,
	R,
	Left,
	Right,
	Up,
	Down,
	Start,
	Select,

	Count
};

/*------------------------------------------------------------
	キーマップ構造体
------------------------------------------------------------*/
struct KEY_MAP
{
	KEY Key{};
	BUTTON Button{};
};

/*============================================================
*	@class	: Input
*	@brief	: 共通キーバインド・入力
*============================================================*/
class Input final
{
private:
	static inline std::map<InputKey, KEY_MAP> mKeyMap{};

private:
	Input() = delete;

public:
	static void Initialize();
	static void Update();

	// ボタン入力
	static bool IsPressed(InputKey input, int index = 0);
	static bool IsTriggered(InputKey input, int index = 0);
	static bool IsReleaseed(InputKey input, int index = 0);

	// アナログ入力
	static float GetAxisX(bool digitalEnable = true, int index = 0);
	static float GetAxisY(bool digitalEnable = true, int index = 0);

	// アプリケーション側からバインドを書き換える
	static void SetKeyMap(InputKey input, KEY key, BUTTON button);
};