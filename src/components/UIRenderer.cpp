/*============================================================
*	@file	 : UIRenderer.cpp
*	@brief	 : UI描画コンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/13
*	@updated : 2026/09/16
*============================================================*/
#include "UIRenderer.h"
#include "Texture.h"
#include "TextureManager.h"
#include "BufferManager.h"
#include "DeviceManager.h"
#include "GameObject.h"
#include "D3D11Config.h"
#include <d3d11.h>

void UIRenderer::Draw() const
{
	Renderer::Begin();

	Bind();

	D3D11::BufferManager::getInstance().Set2DMatrix();
	D3D11::BufferManager::getInstance().SetWorldMatrix(getWorldMatrix());

	// マテリアル設定
	Element::MATERIAL material{};
	material.Diffuse = mColor;
	material.TextureEnable = static_cast<bool>(_mTexture != nullptr);
	D3D11::BufferManager::getInstance().SetMaterial(material);

	// パラメータ設定
	D3D11::BufferManager::getInstance().SetParameter(mParameter);

	mCanvas.Bind();

	if (material.TextureEnable) {
		_mTexture->Bind();
	}

	// UI描画
	mCanvas.Draw();

	Renderer::End();
}

void UIRenderer::Draw(const Transform& transform)
{
	Renderer::Begin();

	Bind();

	D3D11::BufferManager::getInstance().Set2DMatrix();
	D3D11::BufferManager::getInstance().SetWorldMatrix(transform.GetWorldMatrix());

	// マテリアル設定
	Element::MATERIAL material{};
	material.Diffuse = mColor;
	material.TextureEnable = static_cast<bool>(_mTexture != nullptr);
	D3D11::BufferManager::getInstance().SetMaterial(material);

	mCanvas.Bind();

	if (material.TextureEnable) {
		_mTexture->Bind();
	}

	mCanvas.Draw();

	Renderer::End();
}

DirectX::XMMATRIX UIRenderer::getWorldMatrix() const
{
	// ワールド行列取得
	return _mOwner->GetTransform().GetWorldMatrix();
}

UIRenderer* UIRenderer::LoadTexture(const char* fileName, bool isMip)
{
	// テクスチャ読み込み
	_mTexture = TextureManager::getInstance().Load(fileName, isMip);
	return this;
}
