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
enum class Button : int
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
	キーバインド構造体
------------------------------------------------------------*/
struct KEY_BIND
{
	Key BindKey{};
	Pad BindButton{};
};

/*============================================================
*	@class	: Input
*	@brief	: 共通キーバインド・入力
*============================================================*/
class Input final
{
private:
	static inline std::map<Button, KEY_BIND> mKeyBinds{};

private:
	Input() = delete;

public:
	static void Initialize();
	static void Update();

	// ボタン入力
	static bool GetPress(Button button, int index = 0);
	static bool GetTrigger(Button button, int index = 0);
	static bool GetRelease(Button button, int index = 0);

	// アナログ入力
	static float GetAxisX(int index = 0);
	static float GetAxisY(int index = 0);

	// アプリケーション側からバインドを書き換える
	static void SetKeyBind(Button button, Key key, Pad padButton);
};