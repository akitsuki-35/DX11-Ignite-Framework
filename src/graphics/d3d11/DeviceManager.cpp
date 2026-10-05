/*============================================================
*	@file	 : DeviceManager.cpp
*	@brief	 : DirectX11デバイス管理
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/07/13
*	@updated : 2026/10/05
*============================================================*/
#include "DeviceManager.h"
#include "SystemWindow.h"
#include "Config.h"
#include "D3D11Config.h"

bool D3D11::DeviceManager::Initialize()
{
	// デバイス・スワップチェーン生成
	if (!generateDeviceAndSwapChain()) { return false; }

	// デプスステンシルビュー生成
	if (!generateDepthStencilView()) { return false; }

	// レンダーステート生成
	if (!generateDepthStencilState()) { return false; }
	if (!generateBlendState()) { return false; }
	if (!generateRasterizerState()) { return false; }
	if (!generateSamplerState()) { return false; }

	// レンダーステート登録
	renderStateRegister();
    
	return true;
}

void D3D11::DeviceManager::Fainlize()
{
	_mContext->ClearState();
	_mContext->Flush();
}

bool D3D11::DeviceManager::generateDeviceAndSwapChain()
{
	HRESULT hr = S_OK;

	/*--------------------------------------------------
		スワップチェーン設定
	----------------------------------------------------*/
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = Screen::WIDTH;
	swapChainDesc.BufferDesc.Height = Screen::HEIGHT;
	swapChainDesc.BufferDesc.Format = D3D11::BACKBUFFER_FORMAT;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = System::Window::getInstance().GetHandle();
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;

	// 実行フューチャーレベルを一時保存
	D3D_FEATURE_LEVEL level{};

	/*--------------------------------------------------
		スワップチェーン生成
	----------------------------------------------------*/
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		D3D11::FEATURE_LEVELS,
		_countof(D3D11::FEATURE_LEVELS),
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&_mSwapChain,
		&_mDevice,
		&level,
		&_mContext
	);

	if (FAILED(hr)) { return false; }
	else { mFutureLevel = level; }

	/*--------------------------------------------------
		レンダーターゲットビュー作成
	----------------------------------------------------*/
	ComPtr<ID3D11Texture2D> renderTarget{};
	hr = _mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &renderTarget);
	if (FAILED(hr)) { return false; }

	hr = _mDevice->CreateRenderTargetView(renderTarget.Get(), nullptr, &_mRenderTargetView);
	if (FAILED(hr)) { return false; }
	
	return true;
}

bool D3D11::DeviceManager::generateDepthStencilView()
{
	HRESULT hr = S_OK;

	/*--------------------------------------------------
		デプスステンシルバッファ作成
	----------------------------------------------------*/
	D3D11_TEXTURE2D_DESC depthDesc{};
	depthDesc.Width = Screen::WIDTH;
	depthDesc.Height = Screen::HEIGHT;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> depthBuffer;
	hr = _mDevice->CreateTexture2D(&depthDesc, nullptr, &depthBuffer);
	if (FAILED(hr)) { return false; }

	/*--------------------------------------------------
		デプスステンシルビュー作成
	----------------------------------------------------*/
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = 0;

	hr = _mDevice->CreateDepthStencilView(depthBuffer.Get(), &depthStencilViewDesc, &_mDepthStencilView);
	if (FAILED(hr)) { return false; }

	ID3D11RenderTargetView* const rtvs[] = { _mRenderTargetView.Get() };
	_mContext->OMSetRenderTargets(1, rtvs, _mDepthStencilView.Get());

	assert(_mRenderTargetView);
	assert(_mDepthStencilView);

	return true;
}

bool D3D11::DeviceManager::generateDepthStencilState()
{
	HRESULT hr = S_OK;

	/*--------------------------------------------------
		デプスステンシルステート設定
	----------------------------------------------------*/
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = FALSE;

	// 深度有効
	hr = _mDevice->CreateDepthStencilState(&depthStencilDesc, &_mDepthEnable);
	if (FAILED(hr)) { return false; }

	// テストのみ
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = _mDevice->CreateDepthStencilState(&depthStencilDesc, &_mDepthTestOnly);
	if (FAILED(hr)) { return false; }

	// 深度無効
	depthStencilDesc.DepthEnable = FALSE;
	hr = _mDevice->CreateDepthStencilState(&depthStencilDesc, &_mDepthDisable);
	if (FAILED(hr)) { return false; }

	_mContext->OMSetDepthStencilState(_mDepthEnable.Get(), NULL);

	return true;
}

bool D3D11::DeviceManager::generateBlendState()
{
	HRESULT hr = S_OK;

	/*--------------------------------------------------
		ブレンドステート設定
	----------------------------------------------------*/
	D3D11_BLEND_DESC blendDesc{};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// 通常（アルファブレンド）
	hr = _mDevice->CreateBlendState(&blendDesc, &_mBlendDefault);
	if (FAILED(hr)) { return false; }

	// 加算
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	hr = _mDevice->CreateBlendState(&blendDesc, &_mBlendAdd);
	if (FAILED(hr)) { return false; }

	// 乗算
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	hr = _mDevice->CreateBlendState(&blendDesc, &_mBlendMultiply);
	if (FAILED(hr)) { return false; }

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	_mContext->OMSetBlendState(_mBlendDefault.Get(), blendFactor, 0xffffffff);

	return true;
}

bool D3D11::DeviceManager::generateRasterizerState()
{
	HRESULT hr = S_OK;

	/*--------------------------------------------------
		ラスタライザステート設定
	----------------------------------------------------*/
	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;

	// ソリッド
	hr = _mDevice->CreateRasterizerState(&rasterizerDesc, &_mRasterSolid);
	if (FAILED(hr)) { return false; }

	// ワイヤーフレーム
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	hr = _mDevice->CreateRasterizerState(&rasterizerDesc, &_mRasterWireframe);
	if (FAILED(hr)) { return false; }

	_mContext->RSSetState(_mRasterSolid.Get());

	return true;
}

bool D3D11::DeviceManager::generateSamplerState()
{
	HRESULT hr = S_OK;

	/*--------------------------------------------------
		サンプラーステート設定
	----------------------------------------------------*/
	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MaxAnisotropy = 4;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Anisotropic
	hr = _mDevice->CreateSamplerState(&samplerDesc, &_mSamplerAnisotropic);
	if (FAILED(hr)) { return false; }

	// Linear
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	hr = _mDevice->CreateSamplerState(&samplerDesc, &_mSamplerLinear);
	if (FAILED(hr)) { return false; }

	// Point
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	hr = _mDevice->CreateSamplerState(&samplerDesc, &_mSamplerPoint);
	if (FAILED(hr)) { return false; }

	_mContext->PSSetSamplers(0, 1, _mSamplerAnisotropic.GetAddressOf());

	return true;
}

void D3D11::DeviceManager::renderStateRegister()
{
	// 深度ステート登録
	RenderState::Depth::Enable = _mDepthEnable.Get();
	RenderState::Depth::TestOnly = _mDepthTestOnly.Get();
	RenderState::Depth::Disable = _mDepthDisable.Get();

	// ブレンドステート登録
	RenderState::Blend::Default = _mBlendDefault.Get();
	RenderState::Blend::Add = _mBlendAdd.Get();
	RenderState::Blend::Multiply = _mBlendMultiply.Get();

	// ラスタライザステート登録
	RenderState::Raster::Solid = _mRasterSolid.Get();
	RenderState::Raster::Wireframe = _mRasterWireframe.Get();

	// サンプラーステート登録
	RenderState::Sampler::Anisotropic = _mSamplerAnisotropic.Get();
	RenderState::Sampler::Linear = _mSamplerLinear.Get();
	RenderState::Sampler::Point = _mSamplerPoint.Get();
}

void D3D11::DeviceManager::SetDepthStencilState(ID3D11DepthStencilState* depthState)
{
	// デプスステンシルステート設定
	_mContext->OMSetDepthStencilState(depthState, NULL);
}

void D3D11::DeviceManager::SetBlendState(ID3D11BlendState* blendState)
{
	// ブレンドステート設定
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	_mContext->OMSetBlendState(blendState, blendFactor, 0xffffffff);
}

void D3D11::DeviceManager::SetRasterizerState(ID3D11RasterizerState* rasterizerState)
{
	// ラスタライザステート設定
	_mContext->RSSetState(rasterizerState);
}

void D3D11::DeviceManager::SetSamplerState(ID3D11SamplerState* samplerState)
{
	// サンプラーステート設定
	_mContext->PSSetSamplers(0, 1, &samplerState);
}