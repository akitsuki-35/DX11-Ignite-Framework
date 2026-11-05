/*============================================================
*	@file	 : DeviceManager.h
*	@brief	 : DirectX11デバイス管理
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/07/13
*	@updated : 2026/10/15
*============================================================*/
#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace D3D11 {
/*============================================================
*	@class	: D3D11::DeviceManager
*	@brief	: DirectX11デバイス管理
*============================================================*/
	class DeviceManager final
	{
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

	/*--------------------------------------------------
		Singleton用
	----------------------------------------------------*/
	public:
		static DeviceManager& getInstance() {
			static DeviceManager instance;
			return instance;
		}

	private:
		DeviceManager() = default;
		DeviceManager(const DeviceManager&) = delete;

		DeviceManager& operator=(const DeviceManager&) = delete;
		DeviceManager(DeviceManager&&) = delete;

		DeviceManager& operator=(DeviceManager&&) = delete;
		~DeviceManager() {};

	/*--------------------------------------------------
		メンバ変数・メンバ関数
	----------------------------------------------------*/
	private:
		// 実行フューチャーレベル
		D3D_FEATURE_LEVEL mFutureLevel{};

		// DX11デバイス
		Microsoft::WRL::ComPtr<ID3D11Device> _mDevice{};
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> _mContext{};
		Microsoft::WRL::ComPtr<IDXGISwapChain> _mSwapChain{};

		// レンダーターゲット
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _mRenderTargetView{};
		
		// 深度バッファ
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _mDepthStencilView{};

		// 深度ステート
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  _mDepthEnable{};
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  _mDepthTestOnly{};
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  _mDepthDisable{};

		// ブレンドステート
		Microsoft::WRL::ComPtr<ID3D11BlendState>  _mBlendDefault{};
		Microsoft::WRL::ComPtr<ID3D11BlendState>  _mBlendAdd{};
		Microsoft::WRL::ComPtr<ID3D11BlendState>  _mBlendMultiply{};

		// ラスタライザステート
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> _mRasterSolid{};
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> _mRasterNoCull{};
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> _mRasterWireframe{};

		// サンプラーステート
		Microsoft::WRL::ComPtr<ID3D11SamplerState> _mSamplerAnisotropic{};
		Microsoft::WRL::ComPtr<ID3D11SamplerState> _mSamplerLinear{};
		Microsoft::WRL::ComPtr<ID3D11SamplerState> _mSamplerPoint{};

	public:
		bool Initialize();
		void Fainlize();

	private:
		// デバイス・ステート生成
		bool generateDeviceAndSwapChain();
		bool generateDepthStencilView();
		bool generateDepthStencilState();
		bool generateBlendState();
		bool generateRasterizerState();
		bool generateSamplerState();
		void renderStateRegister();

	public:
		// セッター
		void SetDepthStencilState(ID3D11DepthStencilState* depthState);
		void SetBlendState(ID3D11BlendState* blendState);
		void SetRasterizerState(ID3D11RasterizerState* rasterizerState);
		void SetSamplerState(ID3D11SamplerState* samplerState);

		// ゲッター
		ID3D11Device* GetDevice() const { return _mDevice.Get(); }
		ID3D11DeviceContext* GetContext() const { return _mContext.Get(); }
		IDXGISwapChain* GetSwapChain() const { return _mSwapChain.Get(); }
		ID3D11RenderTargetView* GetRenderTargetView() const { return _mRenderTargetView.Get(); }
		ID3D11DepthStencilView* GetDepthStencilView() const { return _mDepthStencilView.Get(); }
	};
}