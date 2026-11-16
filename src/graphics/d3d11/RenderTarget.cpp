/*============================================================
*	@file	 : RenderTarget.cpp
*	@brief	 : 描画出力先一式
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/15
*	@updated : 2026/11/16
*============================================================*/
#include "RenderTarget.h"

bool D3D11::RenderTarget::Create(const RT_DESC& desc)
{
	/*------------------------------------------------------------
		Desc検査
	------------------------------------------------------------*/
	// サイズが不正
	if (desc.Width == 0 || desc.Height == 0) {
		return false;
	}
	// DX11のレンダーターゲット同時バインド数を超えている
	if (desc.ColorCount > MAX_COLOR_TARGETS) {
		return false;
	}
	// カラーも深度も存在しない
	if (desc.ColorCount == 0 && !desc.UseDepth) {
		return false;
	}
	// カラーを使用する場合はフォーマットが必要
	if (desc.ColorCount > 0 && desc.Format == DXGI_FORMAT_UNKNOWN) {
		return false;
	}

	// 再初期化時の既存リソースを破棄
	Reset();

	/*------------------------------------------------------------
		カラーテクスチャ生成
	------------------------------------------------------------*/
	// 必要な個数分の領域を確保
	_mColors.reserve(desc.ColorCount);

	// 個数分の生成処理
	for (size_t i = 0; i < desc.ColorCount; i++) {
		auto colorTex = std::make_unique<ColorTexture>();
		if (!colorTex->Initialize(desc.Width, desc.Height, desc.ColorMip, desc.Format)) {
			Reset();
			return false;
		}

		_mColors.push_back(std::move(colorTex));
	}

	/*------------------------------------------------------------
		深度テクスチャ生成
	------------------------------------------------------------*/
	if (desc.UseDepth) {
		auto depthTex = std::make_unique<DepthTexture>();
		if (!depthTex->Initialize(desc.Width, desc.Height, desc.DepthMip)) {
			Reset();
			return false;
		}

		_mDepth = std::move(depthTex);
	}

	// 全リソース生成が正常に行われたら、キー名とサイズを確定させて初期化を完了する
	mKeyName = desc.KeyName;
	mSize = { desc.Width, desc.Height };

	return true;
}

void D3D11::RenderTarget::Reset()
{
	// 各リソースの破棄
	_mColors.clear();
	_mDepth.reset();

	mKeyName = "";
	mSize = { 0, 0 };
}

void D3D11::RenderTarget::ClearColor(const DirectX::XMFLOAT4& color, int index) const
{
	// カラーテクスチャをクリア
	if (index >= _mColors.size()) {
		return;
	}

	if (index <= -2) {
		return;
	}

	// インデックスが指定されていない場合は全カラーテクスチャをクリア
	if (index == -1) {
		for (const auto& colorTex : _mColors) {

			if (colorTex) {
				colorTex->Clear(color);
			}
		}
	}
	// インデックスが指定されている場合は指定インデックスをクリア
	else {
		const auto& colorTex = _mColors[index];

		if (colorTex) {
			colorTex->Clear(color);
		}
	}
}

void D3D11::RenderTarget::ClearDepth(float depth, UINT8 stencil) const
{
	// 深度テクスチャをクリア
	if (!_mDepth) {
		return;
	}

	_mDepth->Clear(depth, stencil);
}

D3D11::ColorTexture* D3D11::RenderTarget::GetColorTexture(int index) const
{
	// 指定インデックスのカラーテクスチャ取得
	if (index >= _mColors.size()) {
		return nullptr;
	}

	return _mColors[index].get();
}

D3D11::DepthTexture* D3D11::RenderTarget::GetDepthTexture() const
{
	// 深度テクスチャ取得
	if (!_mDepth) {
		return nullptr;
	}

	return _mDepth.get();
}

ID3D11RenderTargetView* D3D11::RenderTarget::GetRTV(int index) const
{
	// 指定インデックスのRTV取得
	const ColorTexture* colorTex = GetColorTexture(index);

	if (!colorTex) {
		return nullptr;
	}

	return colorTex->GetRTV();
}

ID3D11DepthStencilView* D3D11::RenderTarget::GetDSV() const
{
	// DSV取得
	if (!_mDepth) {
		return nullptr;
	}

	return _mDepth->GetDSV();
}

UINT D3D11::RenderTarget::GetColorCount() const
{
	// カラーテクスチャ枚数取得
	return static_cast<UINT>(_mColors.size());
}
