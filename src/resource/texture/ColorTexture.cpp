/*============================================================
*	@file	 : ColorTexture.cpp
*	@brief	 : レンダリングカラーテクスチャ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/12
*	@updated : 2026/11/15
*============================================================*/
#include "ColorTexture.h"
#include "DeviceManager.h"
#include <d3d11.h>

bool D3D11::ColorTexture::Initialize(UINT width, UINT height, UINT mip, DXGI_FORMAT format)
{
    // 引数が不正な場合は処理しない
    if (width <= 0 || height <= 0) {
        return false;
    }

    if (format == DXGI_FORMAT_UNKNOWN) {
        return false;
    }

    // デバイス保持
    auto device = D3D11::DeviceManager::getInstance().GetDevice();
    if (!device) {
        return false;
    }

    // 一度初期状態にする
    _mSRV.Reset();
    _mRTV.Reset();
    _mTexture.Reset();
    mSize = { 0, 0 };

    /*------------------------------------------------------------
	    レンダリングテクスチャ作成
    ------------------------------------------------------------*/
    D3D11_TEXTURE2D_DESC t2d{};
    t2d.Width = width;
    t2d.Height = height;
    t2d.Format = format;
    t2d.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    t2d.ArraySize = 1;
    t2d.SampleDesc.Count = 1;
    t2d.Usage = D3D11_USAGE_DEFAULT;
    t2d.CPUAccessFlags = 0;
    t2d.MipLevels = mip;
    t2d.MiscFlags = (mip == 0) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

    HRESULT hr = device->CreateTexture2D(&t2d, nullptr, _mTexture.GetAddressOf());
    if (FAILED(hr)) {
        reset();
        return false;
    }

    /*------------------------------------------------------------
        RTV作成
    ------------------------------------------------------------*/
    CD3D11_RENDER_TARGET_VIEW_DESC rtvd{};
    rtvd.Format = format;
    rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    hr = device->CreateRenderTargetView(_mTexture.Get(), &rtvd, _mRTV.GetAddressOf());
    if (FAILED(hr)) {
        reset();
        return false;
    }

    /*------------------------------------------------------------
        SRV作成
    ------------------------------------------------------------*/
    D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
    srvd.Format = format;
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvd.Texture2D.MipLevels = (mip == 0) ? static_cast<UINT>(-1) : mip;

    hr = device->CreateShaderResourceView(_mTexture.Get(), &srvd, _mSRV.GetAddressOf());
    if (FAILED(hr)) {
        reset();
        return false;
    }

    // サイズを確定
    mSize = { width, height };

    return true;
}

void D3D11::ColorTexture::Clear(const DirectX::XMFLOAT4& color)
{
    // レンダーターゲットのクリア
    const float clearColor[4] = { color.x, color.y, color.z, color.w };
    D3D11::DeviceManager::getInstance().GetContext()->ClearRenderTargetView(_mRTV.Get(), clearColor);
}

void D3D11::ColorTexture::reset()
{
    // リセット処理
    _mSRV = nullptr;
    _mRTV = nullptr;
    _mTexture = nullptr;
    mSize = { 0, 0 };
}