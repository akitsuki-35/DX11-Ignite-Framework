/*============================================================
*	@file	 : SpriteRenderer.cpp
*	@brief	 : 板ポリゴン描画コンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/13
*	@updated : 2026/09/16
*============================================================*/
#include "SpriteRenderer.h"
#include "Texture.h"
#include "TextureManager.h"
#include "BufferManager.h"
#include "GameObject.h"

void SpriteRenderer::Draw() const
{
	Renderer::Begin();

	Bind();

	D3D11::BufferManager::getInstance().SetWorldMatrix(getWorldMatrix());

	// マテリアル設定
	Element::MATERIAL material{};
	material.Diffuse = mColor;
	material.TextureEnable = static_cast<bool>(_mTexture != nullptr);
	D3D11::BufferManager::getInstance().SetMaterial(material);

	// パラメータ設定
	D3D11::BufferManager::getInstance().SetParameter(mParameter);

	mMesh.Bind();

	if (material.TextureEnable) {
		_mTexture->Bind();
	}

	// メッシュ描画
	mMesh.Draw();

	Renderer::End();
}

DirectX::XMMATRIX SpriteRenderer::getWorldMatrix() const
{
	// ワールド行列取得
	return _mOwner->GetTransform().GetWorldMatrix();
}

SpriteRenderer* SpriteRenderer::LoadTexture(const char* fileName, bool isMip)
{
	// テクスチャ読み込み
	_mTexture = TextureManager::getInstance().Load(fileName, isMip);
	return this;
}
