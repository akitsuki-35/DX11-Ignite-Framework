/*============================================================
*	@file	 : TextureManager.cpp
*	@brief	 : テクスチャ管理
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/07/30
*	@updated : 2026/09/26
*============================================================*/
#include "TextureManager.h"
#include "Texture.h"
#include "DeviceManager.h"
#include "FileUtility.h"
#include "StringUtility.h"
#include <DirectXTex/DirectXTex.h>

using namespace DirectX;

Texture* TextureManager::Load(const char* texturePath, bool isMip)
{
	// キャッシュ取得用にパスを正規化
	std::string key = Utility::File::normalizePath(texturePath);

	// キャッシュが存在すれば返す
	auto it = mTextures.find(key);

	if (it != mTextures.end()) {
		return it->second.get();
	}

	// テクスチャリソース生成
	std::unique_ptr<Texture> texture = std::make_unique<Texture>();

	if (!generateTexture(*texture, key, isMip)) {
		return nullptr;
	}

	// テクスチャを一時変数に格納
	Texture* t = texture.get();

	// コンテナへ登録
	mTextures.emplace(key, std::move(texture));

	return t;
}

void TextureManager::Clear()
{
	mTextures.clear();
}

bool TextureManager::generateTexture(Texture& texture, const std::string& path, bool isMip)
{
	// パスをstd::wstringに変換
	const std::wstring wide = Utility::String::toWideString(path);

	// テクスチャ読込
	TexMetadata metaData{};
	ScratchImage image{};

	HRESULT hr = LoadFromWICFile(wide.c_str(), WIC_FLAGS_NONE, &metaData, image);
	if (FAILED(hr)) return false;

	if (isMip) {
		// ミップマップ生成
		ScratchImage mipChain{};
		GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(),
			TEX_FILTER_DEFAULT, 0, mipChain);

		hr = CreateShaderResourceView(D3D11::DeviceManager::getInstance().GetDevice(),
			mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), texture._mSRV.GetAddressOf());
		if (FAILED(hr)) return false;
	}
	else {
		hr = CreateShaderResourceView(D3D11::DeviceManager::getInstance().GetDevice(),
			image.GetImages(), image.GetImageCount(), metaData, texture._mSRV.GetAddressOf());
		if (FAILED(hr)) return false;
	}

	texture.mSize = { static_cast<UINT>(metaData.width), static_cast<UINT>(metaData.height) };

	return true;
}
