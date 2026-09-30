/*============================================================
*	@file	 : FontManager.h
*	@brief	 : フォント管理
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/11
*	@updated : 2026/09/30
*============================================================*/
#pragma once

#include "FontLoader.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <wrl/client.h>

/*------------------------------------------------------------
	前方宣言
------------------------------------------------------------*/
class Texture;

/*------------------------------------------------------------
	文字テクスチャデータ
------------------------------------------------------------*/
struct GLYPH
{
	// テクスチャ本体
	std::shared_ptr<Texture> Texture{ nullptr };

	// 左右余白
	int BearingX{ 0 };

	// 上下余白
	int BearingY{ 0 };

	// 字間
	int Advance{ 0 };
};

/*------------------------------------------------------------
	文字探索用キー
------------------------------------------------------------*/
struct GLYPHKEY
{
	// フォント
	FONT* Font{};

	// 文字
	uint32_t Codepoint{};
	
	// フォントサイズ
	int Size{};

	bool operator==(const GLYPHKEY& o) const {
		return Font == o.Font && Codepoint == o.Codepoint && Size == o.Size;
	}
};

// ハッシュ化
template <>
struct std::hash<GLYPHKEY> {
	size_t operator()(const GLYPHKEY& k) const {

		size_t h1 = std::hash<const FONT*>()(k.Font);
		size_t h2 = std::hash<uint32_t>()(k.Codepoint);
		size_t h3 = std::hash<int>()(k.Size);

		size_t seed = h1;
		seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
};

/*============================================================
*	@class	: FontManager
*	@brief	: フォントのロード・管理
*============================================================*/
class FontManager final
{
	friend class FontLoader;

/*--------------------------------------------------
	Singleton用
----------------------------------------------------*/
public:
	static FontManager& getInstance() {
		static FontManager  instance;
		return instance;
	}

private:
	FontManager() = default;
	FontManager(const FontManager&) = delete;

	FontManager& operator=(const FontManager&) = delete;
	FontManager(FontManager&&) = delete;

	FontManager& operator=(FontManager&&) = delete;
	~FontManager() {};

/*--------------------------------------------------
	メンバ変数・メンバ関数
----------------------------------------------------*/
private:
	// フォントコンテナ
	std::unordered_map<std::string, std::unique_ptr<FONT>> mFonts{};

	// 文字テクスチャキャッシュ
	std::unordered_map<GLYPHKEY, std::unique_ptr<GLYPH>> mAtlas{};

	// DirectWriteファクトリ
	Microsoft::WRL::ComPtr<IDWriteFactory> _mFactory{ nullptr };

public:
	// ファクトリ取得
	void Initialize(IDWriteFactory* factory) { _mFactory = factory; }

	// フォント取得
	FONT* GetFont(const std::string& keyName);
	
	// 文字テクスチャ取得
	GLYPH* GetGlyph(const GLYPHKEY& key);

	// フォント登録
	FONT* Register(const std::string& keyName, const char* fontPath);

	// クリア
	void Clear();

private:
	// 文字テクスチャ生成
	bool generateGlyph(GLYPH& glyph, const GLYPHKEY& key);
};