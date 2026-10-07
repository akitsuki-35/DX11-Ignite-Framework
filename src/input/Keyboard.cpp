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

bool Keyboard::GetKeyPress(Key keyCode)
{
	return (mKeyState[static_cast<BYTE>(keyCode)] & 0x80) != 0;
}

bool Keyboard::GetKeyTrigger(Key keyCode)
{
	return ((mKeyState[static_cast<BYTE>(keyCode)] & 0x80) && !(mOldKeyState[static_cast<BYTE>(keyCode)] & 0x80));
}

bool Keyboard::GetKeyRelease(Key keyCode)
{
	return ((mOldKeyState[static_cast<BYTE>(keyCode)] & 0x80) && !(mKeyState[static_cast<BYTE>(keyCode)] & 0x80));
}
