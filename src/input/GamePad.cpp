/*============================================================
*	@file	 : GamePad.cpp
*	@brief	 : ゲームパッド入力
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/10/07
*	@updated : 2026/10/07
*============================================================*/
#include "GamePad.h"

void GamePad::Initialize()
{
	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		memset(&mGamePads[i].OldPadState, 0, sizeof(XINPUT_STATE));
		memset(&mGamePads[i].PadState, 0, sizeof(XINPUT_STATE));
		mGamePads[i].Connected = false;
	}
}

void GamePad::Update()
{
	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		mGamePads[i].OldPadState = mGamePads[i].PadState;

		memset(&mGamePads[i].PadState, 0, sizeof(XINPUT_STATE));
		DWORD state = XInputGetState(i, &mGamePads[i].PadState);

		mGamePads[i].Connected = (state == ERROR_SUCCESS);
	}
}

bool GamePad::IsConnected(int index)
{
	if (index < 0 || index >= MAX_CONNECTIONS) {
		return false;
	}
	return mGamePads[index].Connected;
}

bool GamePad::GetButtonPress(WORD button, int index)
{
	if (!IsConnected(index)) {
		return false;
	}

	return (mGamePads[index].PadState.Gamepad.wButtons & button) != 0;
}

bool GamePad::GetButtonTrigger(WORD button, int index)
{
	if (!IsConnected(index)) {
		return false;
	}

	bool old = (mGamePads[index].OldPadState.Gamepad.wButtons & button) != 0;
	bool current = (mGamePads[index].PadState.Gamepad.wButtons & button) != 0;

	return (!old && current);
}

bool GamePad::GetButtonRelease(WORD button, int index)
{
	if (!IsConnected(index)) {
		return false;
	}

	bool old = (mGamePads[index].OldPadState.Gamepad.wButtons & button) != 0;
	bool current = (mGamePads[index].PadState.Gamepad.wButtons & button) != 0;

	return (old && !current);
}

float GamePad::GetLeftStickX(int index)
{
	if (!IsConnected(index)) {
		return false;
	}

	SHORT rawX = mGamePads[index].PadState.Gamepad.sThumbLX;

	// デッドゾーン処理
	if (abs(rawX) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
		return 0.0f;
	}

	// 正規化してreturn
	return (rawX < 0) ? (static_cast<float>(rawX) / 32768.0f) : (static_cast<float>(rawX) / 32767.0f);
}

float GamePad::GetLeftStickY(int index)
{
	if (!IsConnected(index)) {
		return false;
	}

	SHORT rawY = mGamePads[index].PadState.Gamepad.sThumbLY;

	// デッドゾーン処理
	if (abs(rawY) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
		return 0.0f;
	}

	// 正規化してreturn
	return (rawY < 0) ? (static_cast<float>(rawY) / 32768.0f) : (static_cast<float>(rawY) / 32767.0f);
}

float GamePad::GetRightStickX(int index)
{
	if (!IsConnected(index)) {
		return false;
	}

	SHORT rawX = mGamePads[index].PadState.Gamepad.sThumbRX;

	// デッドゾーン処理
	if (abs(rawX) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
		return 0.0f;
	}

	// 正規化してreturn
	return (rawX < 0) ? (static_cast<float>(rawX) / 32768.0f) : (static_cast<float>(rawX) / 32767.0f);
}

float GamePad::GetRightStickY(int index)
{
	if (!IsConnected(index)) {
		return false;
	}

	SHORT rawY = mGamePads[index].PadState.Gamepad.sThumbRY;

	// デッドゾーン処理
	if (abs(rawY) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
		return 0.0f;
	}

	// 正規化してreturn
	return (rawY < 0) ? (static_cast<float>(rawY) / 32768.0f) : (static_cast<float>(rawY) / 32767.0f);
}

float GamePad::GetLeftTrigger(int index)
{
	if (!IsConnected(index)) {
		return 0.0f;
	}

	BYTE rawT = mGamePads[index].PadState.Gamepad.bLeftTrigger;

	// トリガー用のデッドゾーン処理
	if (rawT < XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
		return 0.0f;
	}

	// 正規化してreturn
	return static_cast<float>(rawT) / 255.0f;
}

float GamePad::GetRightTrigger(int index)
{
	if (!IsConnected(index)) {
		return 0.0f;
	}

	BYTE rawT = mGamePads[index].PadState.Gamepad.bRightTrigger;

	// トリガー用のデッドゾーン処理
	if (rawT < XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
		return 0.0f;
	}

	// 正規化してreturn
	return static_cast<float>(rawT) / 255.0f;
}
