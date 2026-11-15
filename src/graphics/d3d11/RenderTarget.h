/*============================================================
*	@file	 : RenderTarget.h
*	@brief	 : 描画出力先一式
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/15
*	@updated : 2026/11/15
*============================================================*/
#pragma once

#include "ColorTexture.h"
#include "DepthTexture.h"
#include <string>
#include <vector>
#include <memory>
#include <d3d11.h>

// バックバッファとは別の描画先となるレンダリングテクスチャの組を提供する

namespace D3D11 {
	/*------------------------------------------------------------
		レンダーターゲット初期化構造体
	------------------------------------------------------------*/
	struct RT_DESC {
		// 取得用のキー名
		std::string KeyName{};

		// 描画先サイズ
		// 全カラーテクスチャと深度テクスチャで一致させる
		UINT Width{ 0 };
		UINT Height{ 0 };

		// 生成カラーテクスチャ枚数
		// この枚数をMRTで一度に書き込む
		// 必ず1～8になるようにする
		size_t ColorCount{ 1 };

		// 深度テクスチャの生成有無
		bool UseDepth{ true };

		// 各テクスチャのミップレベル
		UINT ColorMip{ 1 };
		UINT DepthMip{ 1 };

		// フォーマット
		// 現状は全カラーテクスチャで共通
		DXGI_FORMAT Format{ DXGI_FORMAT_R32G32B32A32_FLOAT };
	};

	/*============================================================
	*	@class	: D3D11::RenderTarget
	*	@brief	: レンダーターゲット
	*============================================================*/
	class RenderTarget final
	{
	public:
		// DX11のレンダーターゲット同時バインド数
		static constexpr UINT MAX_COLOR_TARGETS{ D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT };

	private:
		// 識別用キー名
		std::string mKeyName{};

		// MRT用カラー出力
		std::vector<std::unique_ptr<ColorTexture>> _mColors{};

		// 深度出力
		std::unique_ptr<DepthTexture> _mDepth{};

		// 描画先サイズ
		DirectX::XMUINT2 mSize{};

	public:
		// レンダーターゲット作成
		bool Create(const RT_DESC& desc);

		// リソース破棄
		void Reset();

		// カラーテクスチャのクリア
		// -1なら全てクリアし、インデックス指定があればそのテクスチャのみクリアする
		void ClearColor(const DirectX::XMFLOAT4& color, int index = -1) const;

		// 深度テクスチャのクリア
		void ClearDepth(float depth = 1.0f, UINT8 stencil = 0) const;

		// カラーテクスチャ取得
		const ColorTexture* GetColorTexture(int index = 0) const;

		// 深度テクスチャ取得
		const DepthTexture* GetDepthTexture() const;

		// RTV取得
		ID3D11RenderTargetView* GetRTV(int index = 0) const;

		// DSV取得
		ID3D11DepthStencilView* GetDSV() const;

		// カラーテクスチャ枚数取得
		size_t GetColorCount() const;

		// サイズ取得
		const DirectX::XMUINT2& GetSize() const { return mSize; }
		UINT GetWidth() const { return mSize.x; }
		UINT GetHeight() const { return mSize.y; }
	};
}