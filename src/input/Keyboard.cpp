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

bool Keyboard::IsPressed(KEY keyCode)
{
	// キーが押されている
	for (KeyCode key : keyCode.keys) {
		if ((mKeyState[static_cast<BYTE>(key)] & 0x80) != 0) {
			return true;
		}
	}

	return false;
}

bool Keyboard::IsTriggered(KEY keyCode)
{
	// キーが押された
	for (KeyCode key : keyCode.keys) {
		if (((mKeyState[static_cast<BYTE>(key)] & 0x80) &&
			!(mOldKeyState[static_cast<BYTE>(key)] & 0x80))) {
			return true;
		}
	}

	return false;
}

bool Keyboard::IsReleaseed(KEY keyCode)
{
	// キーが離された
	for (KeyCode key : keyCode.keys) {
		if (((mOldKeyState[static_cast<BYTE>(key)] & 0x80) && 
			!(mKeyState[static_cast<BYTE>(key)] & 0x80))) {
			return true;
		}
	}

	return false;
}
