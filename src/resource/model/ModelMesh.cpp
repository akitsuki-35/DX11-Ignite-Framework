/*============================================================
*	@file	 : ModelMesh.cpp
*	@brief	 : モデル用メッシュ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/02
*	@updated : 2026/11/05
*============================================================*/
#include "ModelMesh.h"
#include "DeviceManager.h"

bool ModelMesh::Create(const std::vector<Element::VERTEX3D>& vertices, const std::vector<uint32_t>& indices)
{
	// メッシュ生成

	mVertices = vertices;
	mIndices = indices;

	// 頂点バッファ生成
	if (!generateVertexBuffer())
		return false;

	// インデックスバッファ生成
	if (!generateIndexBuffer())
		return false;

	return true;
}

void ModelMesh::Bind() const
{
	// 頂点バッファ設定
	UINT stride = sizeof(Element::VERTEX3D);
	UINT offset = 0;
	D3D11::DeviceManager::getInstance().GetContext()->IASetVertexBuffers(0, 1, _mVertexBuffer.GetAddressOf(), &stride, &offset);

	// インデックスバッファ設定
	D3D11::DeviceManager::getInstance().GetContext()->IASetIndexBuffer(_mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// プリミティブトポロジ設定
	D3D11::DeviceManager::getInstance().GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ModelMesh::Draw(const SUBSET& subset) const
{
	// 描画
	D3D11::DeviceManager::getInstance().GetContext()->DrawIndexed(subset.IndexNum, subset.StartIndex, 0);
}

void ModelMesh::DrawInstanced(const SUBSET& subset, uint32_t instance) const
{
	// 描画対象が存在しない場合は描画しない
	if (instance == 0) {
		return;
	}

	ID3D11DeviceContext* context = D3D11::DeviceManager::getInstance().GetContext();

	context->DrawIndexedInstanced(subset.IndexNum, instance, subset.StartIndex, 0, 0);
}

bool ModelMesh::generateVertexBuffer()
{
	auto device = D3D11::DeviceManager::getInstance().GetDevice();

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd{};
	bd.ByteWidth = static_cast<UINT>( sizeof(Element::VERTEX3D) * mVertices.size());
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = mVertices.data();

	device->CreateBuffer(&bd, &sd, _mVertexBuffer.GetAddressOf());

	assert(_mVertexBuffer);
	return true;
}

bool ModelMesh::generateIndexBuffer()
{
	auto device = D3D11::DeviceManager::getInstance().GetDevice();

	// インデックスバッファ生成
	D3D11_BUFFER_DESC bd{};
	bd.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mIndices.size());
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = mIndices.data();

	device->CreateBuffer(&bd, &sd, _mIndexBuffer.GetAddressOf());

	assert(_mIndexBuffer);
	return true;
}