/*============================================================
*	@file	 : ImGuiDebug.cpp
*	@brief	 : ImGuiデバッガー
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/05
*	@updated : 2026/11/05
*============================================================*/
#ifndef NDEBUG
#include "ImGuiDebug.h"
#include "DeviceManager.h"

// ImGui
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

/*============================================================
	使い方

	オブジェクトにImGuiDebug.hをインクルードし、
	以下の書式で記述
	
	ImGui::Begin("Cursor");

	// 処理内容

	ImGui::End();
============================================================*/
const void ImGuiDebug::Initialize(HWND hwnd)
{
	// バージョンチェック
	IMGUI_CHECKVERSION();

	// コンテキスト作成
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	// win32用の初期化
	ImGui_ImplWin32_Initialize(hwnd);

	// DirectX11用の初期化
	ImGui_ImplDX11_Initialize(D3D11::DeviceManager::getInstance().GetDevice(),
		D3D11::DeviceManager::getInstance().GetContext());
}

const void ImGuiDebug::Finalize()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

const void ImGuiDebug::Begin()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

const void ImGuiDebug::End()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
#endif