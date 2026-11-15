/*============================================================
*	@file	 : RenderTexture.h
*	@brief	 : レンダリングテクスチャ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/12
*	@updated : 2026/11/15
*============================================================*/
#pragma once

#include "Texture.h"
#include <dxgiformat.h>

/*------------------------------------------------------------
	前方宣言
------------------------------------------------------------*/
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;

/*============================================================
*	@class	: D3D11::RenderTexture
*	@brief	: レンダリングテクスチャ
*============================================================*/
namespace D3D11 {
	class RenderTexture final : public Texture
	{
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

	private:
		// カラーテクスチャ
		Microsoft::WRL::ComPtr<ID3D11Texture2D> _mTexture{ nullptr };

		// レンダーターゲットビュー
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _mRTV{ nullptr };

	public:
		// 初期化
		bool Initialize(UINT width, UINT height, UINT mip = 1, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT);

		// クリア
		void Clear(const DirectX::XMFLOAT4& color);

		// ゲッター
		ID3D11RenderTargetView* GetRTV() const { return _mRTV.Get(); }

	private:
		// 初期状態にリセット
		void reset();
	};
}