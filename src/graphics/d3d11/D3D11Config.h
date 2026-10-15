/*============================================================
*	@file	 : D3D11Config.h
*	@brief	 : DirectX11用設定定義
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/07/13
*	@updated : 2026/10/15
*============================================================*/
#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

/*============================================================
*	@namespace	: D3D11
*	@brief		: D3D11用ユーティリティ
*============================================================*/
namespace D3D11 {

	// フューチャーレベル設定
	static const D3D_FEATURE_LEVEL FEATURE_LEVELS[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	// バックバッファ設定
	static const DXGI_FORMAT BACKBUFFER_FORMAT{ DXGI_FORMAT_R8G8B8A8_UNORM };

	// 深度バッファ設定
	static const DXGI_FORMAT DEPTHBUFFER_FORMAT{ DXGI_FORMAT_D16_UNORM };

	// クリア色
	static const DirectX::XMFLOAT4 CLEAR_COLOR { 0.0f, 1.0f, 1.0f, 1.0f };

	/*============================================================
	*	@namespace	: D3D11::RenderState
	*	@brief		: 描画ステート
	*============================================================*/
	/*------------------------------------------------------------
		ヘッダーインクルードで値代入に使用可能

		例：D3D11::DeviceManager::getInstance().
			SetDepthStencilState(D3D11::RenderState::Depth::Enable)
	--------------------------------------------------------------*/
	namespace RenderState {

		// 深度ステート
		namespace Depth {
			inline ID3D11DepthStencilState* Enable;
			inline ID3D11DepthStencilState* TestOnly;
			inline ID3D11DepthStencilState* Disable;
		}

		// ブレンドステート
		namespace Blend {
			inline ID3D11BlendState* Default;
			inline ID3D11BlendState* Add;
			inline ID3D11BlendState* Multiply;
		}

		// ラスタライザステート
		namespace Raster {
			inline ID3D11RasterizerState* Solid;
			inline ID3D11RasterizerState* NoCull;
			inline ID3D11RasterizerState* Wireframe;
		}

		// サンプラーステート
		namespace Sampler {
			inline ID3D11SamplerState* Anisotropic;
			inline ID3D11SamplerState* Linear;
			inline ID3D11SamplerState* Point;
		}
	}
}