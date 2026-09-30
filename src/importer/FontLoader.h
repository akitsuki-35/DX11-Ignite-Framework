/*============================================================
*	@file	 : FontLoader.h
*	@brief	 : フォントファイル読み込み
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/06/04
*	@updated : 2026/09/30
*============================================================*/
#pragma once

#include <dwrite.h>
#include <vector>
#include <wrl/client.h>

// フォントデータ構造体
struct FONT {
	// フォントフェース
	Microsoft::WRL::ComPtr<IDWriteFontFace> Face{ nullptr };
	
	// メトリクス
	DWRITE_FONT_METRICS Metrics{};
};

/*============================================================
*	@class	: FontLoader
*	@brief	: フォントファイル読み込み
*============================================================*/
class FontLoader final
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	// 静的な単一クラスとして扱うため、インスタンス化を禁止
	FontLoader() = delete;

public:
	static IDWriteFactory* Initialize();

	// フォントロード
	static bool Load(IDWriteFactory* factory, FONT& font, const char* fontPath);
};