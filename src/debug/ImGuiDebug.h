/*============================================================
*	@file	 : ImGuiDebug.h
*	@brief	 : ImGuiデバッガー
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/05
*	@updated : 2026/11/05
*============================================================*/
#pragma once

#ifndef NDEBUG

#include "ImGui/imgui.h"
#include <Windows.h>

/*============================================================
*	@class	: ImGuiDebug
*	@brief	: ImGuiウィンドウ
*
* 　※Initialize, Fainalize, Draw変更厳禁！！
*============================================================*/
class ImGuiDebug
{
	ImGuiDebug() = delete;

public:
	// 初期化～描画
	static const void Initialize(HWND hwnd);
	static const void Finalize();
	static const void Begin();
	static const void End();
};
#endif