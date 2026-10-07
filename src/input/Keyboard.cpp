/*============================================================
*	@file	 : Keyboard.cpp
*	@brief	 : キーボード入力
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/10/07
*	@updated : 2026/10/07
*============================================================*/
#include "Keyboard.h"

void Keyboard::Initialize()
{
	memset(mOldKeyState, 0, 256);
	memset(mKeyState, 0, 256);
}

void Keyboard::Update()
{
	memcpy(mOldKeyState, mKeyState, 256);

	(void)GetKeyboardState(mKeyState);
}

bool Keyboard::GetKeyPress(BYTE keyCode)
{
	return (mKeyState[keyCode] & 0x80) != 0;
}

bool Keyboard::GetKeyTrigger(BYTE keyCode)
{
	return ((mKeyState[keyCode] & 0x80) && !(mOldKeyState[keyCode] & 0x80));
}

bool Keyboard::GetKeyRelease(BYTE keyCode)
{
	return ((mOldKeyState[keyCode] & 0x80) && !(mKeyState[keyCode] & 0x80));
}
