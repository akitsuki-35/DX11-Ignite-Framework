/*============================================================
*	@file	 : Input.cpp
*	@brief	 : 共通キーバインド・入力
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/10/07
*	@updated : 2026/10/07
*============================================================*/
#include "Input.h"

void Input::Initialize()
{
	Keyboard::Initialize();
	GamePad::Initialize();

	mKeyBinds = {
		{ Button::A, { Key::Z, Pad::A }},
		{ Button::B, { Key::X, Pad::B }},
		{ Button::X, { Key::C, Pad::X }},
		{ Button::Y, { Key::V, Pad::Y }},
		{ Button::L, { Key::Q, Pad::L }},
		{ Button::R, { Key::W, Pad::R }},
		{ Button::Left, { Key::A, Pad::Left }},
		{ Button::Right, { Key::D, Pad::Right }},
		{ Button::Up, { Key::W, Pad::Up }},
		{ Button::Down, { Key::S, Pad::Down }},
		{ Button::Start, { Key::Enter, Pad::Start }},
		{ Button::Select, { Key::Space, Pad::Select }}
	};
}

void Input::Update()
{
	Keyboard::Update();
	GamePad::Update();
}

bool Input::GetPress(Button button, int index)
{
	// ボタンが押されているか
	auto it = mKeyBinds.find(button);
	if (it == mKeyBinds.end()) {
		return false;
	}

	return Keyboard::GetKeyPress(it->second.BindKey) || 
		GamePad::GetButtonPress(it->second.BindButton,index);
}

bool Input::GetTrigger(Button button, int index)
{
	// ボタンが押された瞬間か
	auto it = mKeyBinds.find(button);
	if (it == mKeyBinds.end()) {
		return false;
	}

	return Keyboard::GetKeyTrigger(it->second.BindKey) || 
		GamePad::GetButtonTrigger(it->second.BindButton,index);
}

bool Input::GetRelease(Button button, int index)
{
	// ボタンが離されたか
	auto it = mKeyBinds.find(button);
	if (it == mKeyBinds.end()) {
		return false;
	}

	return Keyboard::GetKeyRelease(it->second.BindKey) ||
		GamePad::GetButtonRelease(it->second.BindButton, index);
}

float Input::GetAxisX(int index)
{
	// 左右アナログ入力

	// コントローラーのスティックを参照
	float stickX = GamePad::GetLeftStickX(index);
	if (abs(stickX) > 0.0f) {
		return stickX;
	}

	// スティックが動いていなければキーボードまたは十字キーバインドを見る
	float keyX = 0.0f;

	if (GetPress(Button::Left, index)) {
		keyX -= 1.0f;
	}

	if (GetPress(Button::Right, index)) {
		keyX += 1.0f;
	}

	return keyX;
}

float Input::GetAxisY(int index)
{
	// 上下アナログ入力

	// コントローラーのスティックを参照
	float stickY = GamePad::GetLeftStickY(index);
	if (abs(stickY) > 0.0f) {
		return stickY;
	}

	// スティックが動いていなければキーボードまたは十字キーバインドを見る
	float keyY = 0.0f;

	if (GetPress(Button::Up, index)) {
		keyY += 1.0f;
	}

	if (GetPress(Button::Down, index)) {
		keyY -= 1.0f;
	}

	return keyY;
}

void Input::SetKeyBind(Button button, Key key, Pad padButton)
{
	mKeyBinds[button].BindKey = key;
	mKeyBinds[button].BindButton = padButton;
}