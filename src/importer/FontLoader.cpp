/*============================================================
*	@file	 : FontLoader.cpp
*	@brief	 : フォントファイル読み込み
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/06/04
*	@updated : 2026/09/30
*============================================================*/
#include "FontLoader.h"
#include "StringUtility.h"
#include <Windows.h>

#pragma comment(lib, "dwrite.lib")

using namespace Microsoft::WRL;

IDWriteFactory* FontLoader::Initialize()
{
	// ファクトリ生成
	ComPtr<IDWriteFactory> factory;
	HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(factory.GetAddressOf()));
	if (FAILED(hr)) {
		return nullptr;
	}

	return factory.Get();
}

bool FontLoader::Load(IDWriteFactory* factory, FONT& font, const char* fontPath)
{
	if (!factory || !fontPath) {
		return false;
	}

	// パスをstd::wstringに変換
	const std::wstring wide = Utility::String::toWideString(fontPath);

	// フォントファイル取得
	ComPtr<IDWriteFontFile> fontFile{};
	HRESULT hr = factory->CreateFontFileReference(wide.c_str(), nullptr, fontFile.GetAddressOf());
	if (FAILED(hr)) {
		return false;
	}

	// フォントフェース作成
	IDWriteFontFile* fontFileArray[] = { fontFile.Get() };
	hr = factory->CreateFontFace(DWRITE_FONT_FACE_TYPE_TRUETYPE, 1, fontFileArray,
		0, DWRITE_FONT_SIMULATIONS_NONE, font.Face.ReleaseAndGetAddressOf());
	if (FAILED(hr)) {
		return false;
	}

	// メトリクス取得
	font.Face->GetMetrics(&font.Metrics);

	return true;
}