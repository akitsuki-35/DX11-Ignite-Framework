/*============================================================
*	@file	 : GridRenderer.h
*	@brief	 : グリッド描画コンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/20
*	@updated : 2026/09/16
*============================================================*/
#pragma once

#include "Renderer.h"
#include <wrl/client.h>

/*------------------------------------------------------------
	前方宣言
------------------------------------------------------------*/
struct ID3D11Buffer;
class Texture;

/*============================================================
*	@class	: GridRenderer
*	@brief	: グリッド描画コンポーネント
*============================================================*/
class GridRenderer : public Renderer
{
private:
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer{};

	// 調点数
	int mVertexCount{};

	// テクスチャ
	Texture* _mTexture{ nullptr };

public:
	GridRenderer(GameObject* owner)
		: Renderer(owner) {
		// 最背面に描画
		mSortKey.layer = Layer::Grid;
	};

	~GridRenderer() override = default;

	// グリッド設定
	GridRenderer* Set(int xCount, int zCount, float size, const char* textureName, bool isMip = false);

	// 描画
	void Draw() const override;

private:
	// ワールド行列取得
	DirectX::XMMATRIX getWorldMatrix() const override;
};