/*============================================================
*	@file	 : Graphics.h
*	@brief	 : 描画システム制御
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/04/21
*	@updated : 2026/11/15
*============================================================*/
#pragma once

#include "Config.h"

/*------------------------------------------------------------
	前方宣言
------------------------------------------------------------*/
namespace D3D11 {
	class ColorTexture;
	class DepthTexture;
}

/*============================================================
*	@class	: D3D11::Graphics
*	@brief	: 描画システム制御
*============================================================*/
namespace D3D11 {
	class Graphics final
	{
	/*--------------------------------------------------
		Singleton用
	----------------------------------------------------*/
	public:
		static Graphics& getInstance() {
			static Graphics instance;
			return instance;
		}

	private:
		Graphics() = default;
		Graphics(const Graphics&) = delete;

		Graphics& operator=(const Graphics&) = delete;
		Graphics(Graphics&&) = delete;

		Graphics& operator=(Graphics&&) = delete;
		~Graphics() {};

	/*--------------------------------------------------
		メンバ変数・メンバ関数
	----------------------------------------------------*/
	public:
		void Initialize();
		void Finalize();
		void Begin();
		void End();
		void SetViewport(float width = Screen::WIDTH, float height = Screen::HEIGHT);

		// レンダリングテクスチャ関連
		void SetRenderTarget(const ColorTexture& colorTex, const DepthTexture& depthTex);
		void SetBackBuffer();
	};
}