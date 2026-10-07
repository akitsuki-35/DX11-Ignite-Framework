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

	// キーマップ初期化
	mKeyMap = {
		{ InputKey::A, { KeyCode::Space || KeyCode::Enter, ButtonCode::A }},
		{ InputKey::B, { KeyCode::C, ButtonCode::B }},
		{ InputKey::X, { KeyCode::Z, ButtonCode::X }},
		{ InputKey::Y, { KeyCode::X, ButtonCode::Y }},
		{ InputKey::L, { KeyCode::Q, ButtonCode::L }},
		{ InputKey::R, { KeyCode::E, ButtonCode::R }},
		{ InputKey::Left, { KeyCode::A, ButtonCode::Left }},
		{ InputKey::Right, { KeyCode::D, ButtonCode::Right }},
		{ InputKey::Up, { KeyCode::W, ButtonCode::Up }},
		{ InputKey::Down, { KeyCode::S, ButtonCode::Down }},
		{ InputKey::Start, { KeyCode::Enter, ButtonCode::Start }},
		{ InputKey::Select, { KeyCode::Space, ButtonCode::Select }}
	};
}

void Input::Update()
{
	Keyboard::Update();
	GamePad::Update();
}

bool Input::IsPressed(InputKey input, int index)
{
	// ボタンが押されているか
	auto it = mKeyMap.find(input);
	if (it == mKeyMap.end()) {
		return false;
	}

	return Keyboard::IsPressed(it->second.Key) || 
		GamePad::IsPressed(it->second.Button,index);
}

bool Input::IsTriggered(InputKey input, int index)
{
	// ボタンが押された瞬間か
	auto it = mKeyMap.find(input);
	if (it == mKeyMap.end()) {
		return false;
	}

	return Keyboard::IsTriggered(it->second.Key) || 
		GamePad::IsTriggered(it->second.Button,index);
}

bool Input::IsReleaseed(InputKey input, int index)
{
	// ボタンが離されたか
	auto it = mKeyMap.find(input);
	if (it == mKeyMap.end()) {
		return false;
	}

	return Keyboard::IsReleaseed(it->second.Key) ||
		GamePad::IsReleaseed(it->second.Button, index);
}

float Input::GetAxisX(bool digitalEnable, int index)
{
	// 左右アナログ入力

	// コントローラーのスティックを参照
	float stickX = GamePad::GetLeftAxisX(index);
	if (abs(stickX) > 0.0f) {
		return stickX;
	}

	// スティックが動いていなければキーボードまたは十字キーバインドを見る
	float keyX = 0.0f;

	if (digitalEnable) {
		if (IsPressed(InputKey::Left, index)) {
			keyX -= 1.0f;
		}

		if (IsPressed(InputKey::Right, index)) {
			keyX += 1.0f;
		}
	}
	else {
		// デジタルボタン無効の場合はパッドの十字キーを判定しない
		if (Keyboard::IsPressed(mKeyMap[InputKey::Left].Key)) {
			keyX -= 1.0f;
		}

		if (Keyboard::IsPressed(mKeyMap[InputKey::Right].Key)) {
			keyX += 1.0f;
		}
	}

	return keyX;
}

float Input::GetAxisY(bool digitalEnable, int index)
{
	// 上下アナログ入力

	// コントローラーのスティックを参照
	float stickY = GamePad::GetLeftAxisY(index);
	if (abs(stickY) > 0.0f) {
		return stickY;
	}

	// スティックが動いていなければキーボードまたは十字キーバインドを見る
	float keyY = 0.0f;

	if (digitalEnable) {
		if (IsPressed(InputKey::Up, index)) {
			keyY += 1.0f;
		}

		if (IsPressed(InputKey::Down, index)) {
			keyY -= 1.0f;
		}
	}
	else {
		// デジタルボタン無効の場合はパッドの十字キーを判定しない
		if (Keyboard::IsPressed(mKeyMap[InputKey::Up].Key)) {
			keyY += 1.0f;
		}

		if (Keyboard::IsPressed(mKeyMap[InputKey::Down].Key)) {
			keyY -= 1.0f;
		}
	}

	return keyY;
}

void Input::SetKeyMap(InputKey input, KEY key, BUTTON padButton)
{
	// キーマップ書き換え
	mKeyMap[input].Key = key;
	mKeyMap[input].Button = padButton;
}