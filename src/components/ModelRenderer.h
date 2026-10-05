/*============================================================
*	@file	 : ModelRenderer.h
*	@brief	 : モデル描画コンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/07/31
*	@updated : 2026/10/05
*============================================================*/
#pragma once

#include "Renderer.h"
#include <filesystem>

/*--------------------------------------------------
	前方宣言
----------------------------------------------------*/
class Model;
class Texture;

/*============================================================
*	@class	: ModelRenderer
*	@brief	: モデル描画
*============================================================*/
class ModelRenderer : public Renderer
{
	// テクスチャ
	struct MODEL_TEXTURES
	{
		// アルベドカラー
		Texture* Albedo{};

		// 法線マップ
		Texture* Normal{};

		// ざらつき
		Texture* Roughness{};

		// 金属感
		Texture* Metalness{};

		// セルシェーディング明るさ値
		Texture* Rump{};
	};

public:
	// テクスチャタイプ
	enum class TextureType
	{
		Albedo,
		Normal,
		Roughness,
		Metalness,
		Ramp
	};

private:
	Model* _mModel{};
	MODEL_TEXTURES mTextures{};
	
	// ディレクトリ(テクスチャ検索用)
	std::filesystem::path mDirectory{};

public:
	ModelRenderer(GameObject* owner)
		: Renderer(owner) {
		// 不透明レイヤーに描画
		mSortKey.layer = Layer::World;
	};

	~ModelRenderer() override = default;

	void Finalize() override {
		_mModel = nullptr;
		mTextures.Albedo = nullptr;
		mTextures.Normal = nullptr;
		mTextures.Roughness = nullptr;
		mTextures.Metalness = nullptr;
		mTextures.Rump = nullptr;
	}

	void Draw() const override;

private:
	// ワールド行列取得
	DirectX::XMMATRIX getWorldMatrix() const override;

public:
	// モデル読み込み
	ModelRenderer* LoadModel(const char* fileName, const bool& isAnimLoad = false);

	// テクスチャ読み込み
	ModelRenderer* LoadTexture(std::string textureName, TextureType type = TextureType::Albedo, bool isMip = false);

	// ゲッター
	Model* GetModel() const{ return _mModel; }

private:
	// マップ用テクスチャセット
	void setMapTextures() const;

	// 外部テクスチャのパスを生成
	std::string converttoTexturePath(const std::string& textureName);
};