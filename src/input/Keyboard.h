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
#include <vector>

/*------------------------------------------------------------
	キーボード名前解決テーブル
------------------------------------------------------------*/
enum class KeyCode : BYTE
{
	None = 0x00,

	V0 = '0',
	V1 = '1',
	V2 = '2',
	V3 = '3',
	V4 = '4',
	V5 = '5',
	V6 = '6',
	V7 = '7',
	V8 = '8',
	V9 = '9',

	A = 'A',
	B = 'B',
	C = 'C',
	D = 'D',
	E = 'E',
	F = 'F',
	G = 'G',
	H = 'H',
	I = 'I',
	J = 'J',
	K = 'K',
	L = 'L',
	M = 'M',
	N = 'N',
	O = 'O',
	P = 'P',
	Q = 'Q',
	R = 'R',
	S = 'S',
	T = 'T',
	U = 'U',
	V = 'V',
	W = 'W',
	X = 'X',
	Y = 'Y',
	Z = 'Z',

	F1 = VK_F1,
	F2 = VK_F2,
	F3 = VK_F3,
	F4 = VK_F4,
	F5 = VK_F5,
	F6 = VK_F6,
	F7 = VK_F7,
	F8 = VK_F8,
	F9 = VK_F9,
	F10 = VK_F10,
	F11 = VK_F11,
	F12 = VK_F12,
	F13 = VK_F13,
	F14 = VK_F14,
	F15 = VK_F15,
	F16 = VK_F16,
	F17 = VK_F17,
	F18 = VK_F18,
	F19 = VK_F19,
	F20 = VK_F20,
	F21 = VK_F21,
	F22 = VK_F22,
	F23 = VK_F23,
	F24 = VK_F24,

	PageUp = VK_PRIOR,
	PageDown = VK_NEXT,
	End = VK_END,
	Home = VK_HOME,
	Left = VK_LEFT,
	Right = VK_RIGHT,
	Up = VK_UP,
	Down = VK_DOWN,
	Insert = VK_INSERT, 
	Delete = VK_DELETE,

	BackSpace = VK_BACK,
	Tab = VK_TAB,
	Enter = VK_RETURN,
	Shift = VK_SHIFT,
	Ctrl = VK_CONTROL,
	Alt = VK_MENU,
	Pause = VK_PAUSE,
	CapsLock = VK_CAPITAL,
	Esc = VK_ESCAPE,
	Space = VK_SPACE,

	Num0 = VK_NUMPAD0,
	Num1 = VK_NUMPAD1,
	Num2 = VK_NUMPAD2,
	Num3 = VK_NUMPAD3,
	Num4 = VK_NUMPAD4,
	Num5 = VK_NUMPAD5,
	Num6 = VK_NUMPAD6,
	Num7 = VK_NUMPAD7,
	Num8 = VK_NUMPAD8,
	Num9 = VK_NUMPAD9,
	Add = VK_ADD,
	Subtract = VK_SUBTRACT,
	Multiply = VK_MULTIPLY,
	Divide = VK_DIVIDE,
	Decimal = VK_DECIMAL,

	Kanji = VK_KANJI || VK_HANJA,
	Convert = VK_CONVERT,
	NonConvert = VK_NONCONVERT
};

/*------------------------------------------------------------
	キー構造体
------------------------------------------------------------*/
struct KEY
{
	std::vector<KeyCode> keys{};

	KEY() = default;
	KEY(KeyCode key) { if (key != KeyCode::None) keys.push_back(key); }
};

/*------------------------------------------------------------
	複数キー対応用オペレーター
------------------------------------------------------------*/
inline KEY operator||(KeyCode a, KeyCode b)
{
	KEY g{};
	g.keys.push_back(a);
	g.keys.push_back(b);
	return g;
}

inline KEY operator||(KEY g, KeyCode b)
{
	g.keys.push_back(b);
	return g;
}

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
	static bool IsPressed(KEY keyCode);

	// キーが押された瞬間？
	static bool IsTriggered(KEY keyCode);

	// キーが離された？
	static bool IsReleaseed(KEY keyCode);
};
