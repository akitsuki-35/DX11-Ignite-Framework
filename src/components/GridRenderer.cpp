/*============================================================
*	@file	 : GridRenderer.cpp
*	@brief	 : グリッド描画コンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/20
*	@updated : 2026/09/16
*============================================================*/
#include "GridRenderer.h"
#include "Texture.h"
#include "TextureManager.h"
#include "DeviceManager.h"
#include "BufferManager.h"
#include "GameObject.h"
#include "Elements.h"

using namespace DirectX;

GridRenderer* GridRenderer::Set(int xCount, int zCount, float size, const char* textureName, bool isMip)
{
	// 頂点数の算出
	const int X_V_COUNT = xCount + 1;
	const int Z_V_COUNT = zCount + 1;
	mVertexCount = (X_V_COUNT * 2) + (Z_V_COUNT * 2);

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Element::VERTEX3D) * mVertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	auto* pV = new Element::VERTEX3D[mVertexCount];

	// 中心座標の算出
	float startX = (xCount * size) * -0.5f;
	float startZ = (zCount * size) * -0.5f;

	// 終点座標の算出
	float endX = (xCount * size) * 0.5f;
	float endZ = (zCount * size) * 0.5f;

	// グリッドの頂点情報の書き込み
	int index = 0;
	for (int x = 0; x < X_V_COUNT; x++) {
		pV[index].Position = { startX + size * x, 0.0f, startZ };
		pV[index].Diffuse = mColor;

		pV[index + 1].Position = { startX + size * x, 0.0f, endZ };
		pV[index + 1].Diffuse = mColor;

		index += 2;
	}

	for (int z = 0; z < Z_V_COUNT; z++) {
		pV[index].Position = { startX, 0.0f, startZ + size * z };
		pV[index].Diffuse = mColor;

		pV[index + 1].Position = { endX, 0.0f, startZ + size * z };
		pV[index + 1].Diffuse = mColor;

		index += 2;
	}

	// 頂点バッファへ流し込むデータの設定
	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = pV;

	D3D11::DeviceManager::getInstance().GetDevice()->CreateBuffer(&bd, &sd, mVertexBuffer.GetAddressOf());

	_mTexture = TextureManager::getInstance().Load(textureName, isMip);

	delete[] pV;

	return this;
}

void GridRenderer::Draw() const
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

	// 頂点バッファ設定
	UINT stride = sizeof(Element::VERTEX3D);
	UINT offset = 0;
	D3D11::DeviceManager::getInstance().GetContext()->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);

	// プリミティブトポロジ設定
	D3D11::DeviceManager::getInstance().GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	if (material.TextureEnable) {
		_mTexture->Bind();
	}

	// 描画命令発行
	D3D11::DeviceManager::getInstance().GetContext()->Draw(mVertexCount, 0);

	Renderer::End();
}

DirectX::XMMATRIX GridRenderer::getWorldMatrix() const
{
	// ワールド行列取得
	return _mOwner->GetTransform().GetWorldMatrix();
}
