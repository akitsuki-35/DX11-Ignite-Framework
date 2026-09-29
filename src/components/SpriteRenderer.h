/*============================================================
*	@file	 : SpriteRenderer.h
*	@brief	 : 板ポリゴン描画コンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/07/24
*	@updated : 2026/09/16
*============================================================*/
#pragma once

#include "Renderer.h"
#include "Mesh.h"

/*------------------------------------------------------------
	前方宣言
------------------------------------------------------------*/
class Texture;

/*============================================================
*	@class	: SpriteRenderer
*	@brief	: 板ポリゴン描画コンポーネント
*============================================================*/
class SpriteRenderer : public Renderer
{
protected:
	// メッシュ
	Mesh mMesh{};

	// テクスチャ
	Texture* _mTexture{ nullptr };

public:
	SpriteRenderer(GameObject* owner)
		: Renderer(owner){
		// 不透明レイヤーに描画
		mSortKey.layer = Layer::World;
	};

	~SpriteRenderer() override = default;

	void Finalize() override {
		_mTexture = nullptr;
		Renderer::Finalize();
	}

	// 描画
	void Draw() const override;

private:
	// ワールド行列取得
	DirectX::XMMATRIX getWorldMatrix() const override;

public:
	// テクスチャ読み込み
	SpriteRenderer* LoadTexture(const char* fileName, bool isMip = false);

	// ゲッター
	Mesh& GetMesh() { return mMesh; }
	Texture* GetTexture() const { return _mTexture; }
};