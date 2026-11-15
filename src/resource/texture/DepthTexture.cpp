/*============================================================
*	@file	 : DepthTexture.cpp
*	@brief	 : 深度テクスチャ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/12
*	@updated : 2026/11/15
*============================================================*/
#include "DepthTexture.h"
#include "DeviceManager.h"
#include <d3d11.h>

bool D3D11::DepthTexture::Initialize(UINT width, UINT height, UINT mip)
{
    // 引数が不正な場合は処理しない
    if (width <= 0 || height <= 0) {
        return false;
    }

    // デバイス保持
    auto device = D3D11::DeviceManager::getInstance().GetDevice();
    if (!device) {
        return false;
    }

    // 一度初期状態にする
    _mSRV.Reset();
    _mDSV.Reset();
    _mTexture.Reset();
    mSize = { 0, 0 };

    /*------------------------------------------------------------
	    深度テクスチャ作成
    ------------------------------------------------------------*/
    D3D11_TEXTURE2D_DESC t2d{};
    t2d.Width = width;
    t2d.Height = height;
    t2d.Format = DXGI_FORMAT_R32_TYPELESS;
    t2d.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    t2d.ArraySize = 1;
    t2d.SampleDesc.Count = 1;
    t2d.Usage = D3D11_USAGE_DEFAULT;
    t2d.CPUAccessFlags = 0;
    t2d.MipLevels = mip;
    t2d.MiscFlags = 0;

    HRESULT hr = device->CreateTexture2D(&t2d, nullptr, _mTexture.GetAddressOf());
    if (FAILED(hr)) {
        reset();
        return false;
    }

    /*------------------------------------------------------------
        DSV作成
    ------------------------------------------------------------*/
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd{};
    dsvd.Format = DXGI_FORMAT_D32_FLOAT;
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    hr = device->CreateDepthStencilView(_mTexture.Get(), &dsvd, _mDSV.GetAddressOf());
    if (FAILED(hr)) {
        reset();
        return false;
    }

    /*------------------------------------------------------------
        SRV作成
    ------------------------------------------------------------*/
    D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
    srvd.Format = DXGI_FORMAT_R32_FLOAT;
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvd.Texture2D.MipLevels = mip;

    hr = device->CreateShaderResourceView(_mTexture.Get(), &srvd, _mSRV.GetAddressOf());
    if (FAILED(hr)) {
        reset();
        return false;
    }

    // サイズを確定
    mSize = { width, height };

    return true;
}

void D3D11::DepthTexture::Clear(float depth, UINT8 stencil) const
{
    if (!_mDSV) {
        return;
    }

    D3D11::DeviceManager::getInstance().GetContext()->ClearDepthStencilView(_mDSV.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
}

void D3D11::DepthTexture::reset()
{
    // リセット処理
    _mSRV = nullptr;
    _mDSV = nullptr;
    _mTexture = nullptr;
    mSize = { 0, 0 };
}