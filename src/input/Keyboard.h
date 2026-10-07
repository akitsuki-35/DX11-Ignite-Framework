/*============================================================
*	@file	 : Keyboard.h
*	@brief	 : キーボード入力
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/10/07
*	@updated : 2026/10/07
*============================================================*/
#pragma once

#include <Windows.h>

/*============================================================
*	@class	: Keyboard
*	@brief	: キーボード入力
*============================================================*/
class Keyboard final
{
private:
	static inline BYTE mOldKeyState[256]{};
	static inline BYTE mKeyState[256]{};

private:
	Keyboard() = delete;

public:
	static void Initialize();
	static void Update();

	// キーが押されている？
	static bool GetKeyPress(BYTE keyCode);

	// キーが押された瞬間？
	static bool GetKeyTrigger(BYTE keyCode);

	// キーが離された？
	static bool GetKeyRelease(BYTE keyCode);
};
