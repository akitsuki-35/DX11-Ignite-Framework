/*============================================================
*	@file	 : DepthTexture.h
*	@brief	 : 深度テクスチャ
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
struct ID3D11DepthStencilView;

/*============================================================
*	@class	: D3D11::DepthTexture
*	@brief	: 深度テクスチャ
*============================================================*/
namespace D3D11 {
	class DepthTexture final : public Texture
	{
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

	private:
		// 深度テクスチャ
		ComPtr<ID3D11Texture2D> _mTexture{ nullptr };

		// 深度ステンシルビュー
		ComPtr<ID3D11DepthStencilView> _mDSV{ nullptr };

	public:
		// 深度テクスチャ初期化
		bool Initialize(UINT width, UINT height, UINT mip);

		// 深度・ステンシルのクリア
		void Clear(float depth, UINT8 stencil) const;

		// ゲッター
		ID3D11DepthStencilView* GetDSV() const { return _mDSV.Get(); }

	private:
		// 初期状態にリセット
		void reset();
	};
}