/*============================================================
*	@file	 : Graphics.cpp
*	@brief	 : 描画システム制御
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/04/21
*	@updated : 2026/11/16
*============================================================*/
#include "Graphics.h"
#include "DeviceManager.h"
#include "BufferManager.h"
#include "FontLoader.h"
#include "FontManager.h"
#include "SystemWindow.h"
#include "RenderTarget.h"

void D3D11::Graphics::Initialize()
{
	// デバイス初期化
	if (!D3D11::DeviceManager::getInstance().Initialize()) {
		return;
	}

	// ビューポート設定
	// デフォルトではウィンドウ幅全体を使用
	SetViewport();

	// 定数バッファ初期化
	D3D11::BufferManager::getInstance().Initialize();

	// DirectWriteファクトリ生成
	FontManager::getInstance().Initialize(FontLoader::Initialize());
}

void D3D11::Graphics::Finalize()
{
	D3D11::DeviceManager::getInstance().Fainlize();
}

void D3D11::Graphics::Begin()
{
	// ループ先頭で呼出
	auto& device = D3D11::DeviceManager::getInstance();

	// 画面クリア
	float clearColor[4] = { 0.2f, 0.5f, 0.1f, 1.0f };
	device.GetContext()->ClearRenderTargetView( device.GetRenderTargetView(), clearColor);
	device.GetContext()->ClearDepthStencilView( device.GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3D11::Graphics::End()
{
	// ループ末尾で呼出
	D3D11::DeviceManager::getInstance().GetSwapChain()->Present(1, 0);
}

void D3D11::Graphics::SetViewport(float width, float height)
{
	// ビューポート設定
	D3D11_VIEWPORT viewport{};
	viewport.Width = (FLOAT)width;
	viewport.Height = (FLOAT)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	D3D11::DeviceManager::getInstance().GetContext()->RSSetViewports(1, &viewport);
}

void D3D11::Graphics::SetRenderTarget(const RenderTarget* rt)
{
	// 描画先をレンダリングテクスチャに切り替え
	if (!rt) {
		return;
	}

	// RTV配列
	ID3D11RenderTargetView* rtvs[D3D11::RenderTarget::MAX_COLOR_TARGETS]{};

	const UINT colorCount = rt->GetColorCount();

	// レンダーターゲットのRTVを取得
	for (UINT i = 0; i < colorCount; ++i) {
		rtvs[i] = rt->GetRTV(i);
	}

	// DSV取得
	ID3D11DepthStencilView* dsv = rt->GetDSV();

	// 描画先を設定
	DeviceManager::getInstance().GetContext()->OMSetRenderTargets(colorCount, 
		colorCount > 0 ? rtvs : nullptr, dsv);
}

void D3D11::Graphics::SetBackBuffer()
{
	auto& dm = D3D11::DeviceManager::getInstance();

	ID3D11RenderTargetView* rtv = dm.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = dm.GetDepthStencilView();

	if (!rtv) {
		return;
	}

	// バックバッファを現在の描画先へ設定
	dm.GetContext()->OMSetRenderTargets(1, &rtv, dsv);
}