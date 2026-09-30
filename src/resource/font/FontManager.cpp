/*============================================================
*	@file	 : FontManager.cpp
*	@brief	 : フォント管理
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/11
*	@updated : 2026/09/30
*============================================================*/
#include "FontManager.h"
#include "FontLoader.h"
#include "DeviceManager.h"
#include "Texture.h"
#include <d3d11.h>
#include <dwrite.h>

using namespace Microsoft::WRL;

FONT* FontManager::GetFont(const std::string& keyName)
{	
    // キャッシュが存在すれば返す
    auto it = mFonts.find(keyName);

    if (it != mFonts.end()) {
        return it->second.get();
    }

    return nullptr;
}

GLYPH* FontManager::GetGlyph(const GLYPHKEY& key)
{
	// キャッシュが存在すれば返す
	auto it = mAtlas.find(key);

	if (it != mAtlas.end()) {
		return it->second.get();
	}

	// 文字テクスチャ生成
	std::unique_ptr<GLYPH> glyph = std::make_unique<GLYPH>();

	if (!generateGlyph(*glyph, key)) {
		return nullptr;
	}

    // 文字テクスチャを一時変数に格納
    GLYPH* g = glyph.get();

    // コンテナへ登録
    mAtlas.emplace(key, std::move(glyph));

	return g;
}

FONT* FontManager::Register(const std::string& keyName, const char* fontPath)
{
    // 登録済みならreturn
    if (mFonts.contains(keyName)) {
        return mFonts[keyName].get();
    }

    // フォント生成
    std::unique_ptr<FONT> font = std::make_unique<FONT>();

    if (!FontLoader::Load(_mFactory.Get(), *font, fontPath)) {
        return nullptr;
    }

    // コンテナへ登録
    mFonts.emplace(keyName, std::move(font));

    return mFonts[keyName].get();
}

void FontManager::Clear()
{
    mAtlas.clear();
    mFonts.clear();
}

bool FontManager::generateGlyph(GLYPH& glyph, const GLYPHKEY& key)
{
    // 文字コードをGlyphインデックスに変換
    UINT16 glyphIndex = 0;
    key.Font->Face->GetGlyphIndices(&key.Codepoint, 1, &glyphIndex);

    // メトリクス取得
    DWRITE_GLYPH_METRICS metrics{};
    key.Font->Face->GetDesignGlyphMetrics(&glyphIndex, 1, &metrics);
    
    // サイズをピクセル単位に変換
    float designUnitsPerEm = key.Font->Metrics.designUnitsPerEm;
    float fontSize = static_cast<float>(key.Size);

    // GlyphRun作成
    DWRITE_GLYPH_RUN glyphRun{};
    glyphRun.fontFace = key.Font->Face.Get();
    glyphRun.fontEmSize = static_cast<float>(key.Size);
    glyphRun.glyphCount = 1;
    glyphRun.glyphIndices = &glyphIndex;

    // ラスタライザ作成
    ComPtr<IDWriteGlyphRunAnalysis> analysis = nullptr;
    _mFactory->CreateGlyphRunAnalysis(&glyphRun, 1.0f, nullptr, DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC,
        DWRITE_MEASURING_MODE_NATURAL, 0, 0, analysis.GetAddressOf());

    // Bitmapサイズ取得
    RECT bounds{};
    analysis->GetAlphaTextureBounds(DWRITE_TEXTURE_CLEARTYPE_3x1, &bounds);
    int width = bounds.right - bounds.left;
    int height = bounds.bottom - bounds.top;

    // 空白文字の特殊処理
    if (width == 0 || height == 0) {
        if (!glyph.Texture) {
            glyph.Texture = std::make_shared<Texture>();
        }
        glyph.Texture->_mSRV = nullptr;
        glyph.Texture->mSize = { 0, 0 };

        glyph.BearingX = bounds.left;
        glyph.BearingY = bounds.top;
        glyph.Advance = static_cast<uint32_t>(metrics.advanceWidth * (fontSize / designUnitsPerEm));

        return true;
    }

    // Bitmap取得
    std::vector<BYTE> buffer(width * height * 3);
    analysis->CreateAlphaTexture(DWRITE_TEXTURE_CLEARTYPE_3x1, &bounds,
        buffer.data(), static_cast<UINT32>(buffer.size()));

    // RGBAに変換
    std::vector<BYTE> rgba(width * height * 4);
    for (int i = 0; i < width * height; i++) {
        BYTE r = buffer[i * 3 + 0];
        BYTE g = buffer[i * 3 + 1];
        BYTE b = buffer[i * 3 + 2];
        BYTE a = (r + g + b) / 3; // 簡易アルファ

        rgba[i * 4 + 0] = 255;
        rgba[i * 4 + 1] = 255;
        rgba[i * 4 + 2] = 255;
        rgba[i * 4 + 3] = a;
    }

    // DirectX11テクスチャ作成
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = width ;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = rgba.data();
    init.SysMemPitch = width * 4;

    ComPtr<ID3D11Texture2D> tex = nullptr;
    HRESULT hr = D3D11::DeviceManager::getInstance().GetDevice()->CreateTexture2D(&desc, &init, tex.GetAddressOf());
    if (FAILED(hr)) {
        return false;
    }

    ComPtr<ID3D11ShaderResourceView> srv = nullptr;
    hr = D3D11::DeviceManager::getInstance().GetDevice()->CreateShaderResourceView(tex.Get(), nullptr, srv.GetAddressOf());
    if (FAILED(hr)) {
        return false;
    }

    if (!glyph.Texture) {
        glyph.Texture = std::make_shared<Texture>();
    }
    
    // パラメータ設定
    glyph.Texture->_mSRV = srv.Get();
    glyph.Texture->mSize = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
    glyph.BearingX = static_cast<int32_t>(metrics.leftSideBearing * (fontSize / designUnitsPerEm));
    glyph.BearingY = static_cast<int32_t>(metrics.topSideBearing * (fontSize / designUnitsPerEm));
    glyph.Advance = static_cast<uint32_t>(metrics.advanceWidth * (fontSize / designUnitsPerEm) + fontSize / 10);

    return true;
}