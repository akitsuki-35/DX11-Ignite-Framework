/*============================================================
*	@file	 : TextRenderer.h
*	@brief	 : テキスト描画コンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/12
*	@updated : 2026/09/30
*============================================================*/
#pragma once

#include "UIRenderer.h"
#include "Vector2.h"

/*------------------------------------------------------------
	前方宣言
------------------------------------------------------------*/
class Texture;
class Transform;
struct FONT;
struct GLYPH;

/*============================================================
*	@class	: UIRenderer
*	@brief	: UI描画コンポーネント
*============================================================*/
class TextRenderer : public UIRenderer
{
private:
	// フォント
	FONT* _mFont{};

	// 表示文字列
	std::wstring mText{};

	// 1行あたりの文字数
	size_t mCharsPerLine{ 30 };

	// フォントサイズ
	int mSize{ 32 }; 

	// トランスフォーム座標からのオフセット
	Vector2 mOffset{};

	// ドロップシャドウカラー
	DirectX::XMFLOAT4 mShadowColor{ 0.0f, 0.0f, 0.0f, 1.0f };

	// ドロップシャドウの有無
	bool mShadowEnable{ true };

public:
	void Finalize() override {
		_mFont = nullptr;
		UIRenderer::Finalize();
	}

	TextRenderer();

	TextRenderer(GameObject* owner);

	~TextRenderer() override = default;

	// 描画
	void Draw() const override;

private:
	// ドロップシャドウ描画
	void shadowDraw(const GLYPH* glyph, const Transform& transform) const;

	DirectX::XMMATRIX getWorldMatrix() = delete;

public:
	// テクスチャ読み込みは不要なので削除
	UIRenderer* LoadTexture(const char* fileName) = delete;

	// フォントセット
	TextRenderer* SetFont(const std::string& fontName);

	// テキストサイズ更新
	TextRenderer* SetTextSize(const int& size);

	// テキスト更新
	TextRenderer* SetText(const std::string& text);

	// 1行あたりの文字数更新
	TextRenderer* SetCharsPerLine(const size_t& charsPerLine);

	// オフセット更新
	TextRenderer* SetOffset(const Vector2& offset);

	// ドロップシャドウカラー変更
	TextRenderer* SetShadowColor(const DirectX::XMFLOAT4 color);

	// ドロップシャドウの有効無効切り替え
	TextRenderer* SetShadowEnable(const bool& isEnable);

	// オフセット取得
	Vector2 GetOffset() { return mOffset; }
};