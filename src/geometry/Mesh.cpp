/*============================================================
*	@file	 : Mesh.cpp
*	@brief	 : ポリゴンメッシュ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/07/24
*	@updated : 2026/11/04
*============================================================*/
#include "Mesh.h"
#include "DeviceManager.h"
#include "Elements.h"
#include <cassert>
using namespace DirectX;

bool Mesh::CreatePlane(MeshType::Plane::Pivot pivot, MeshType::Plane::Axis axis)
{
	mType = Type::Plane;

	// 頂点情報定義
	MeshType::Plane::DESC desc{};
	desc.pivot = pivot;
	desc.axis = axis;

	std::array<Element::VERTEX3D, 4> vertex = MeshType::Plane::Create(desc);

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Element::VERTEX3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertex.data();

	D3D11::DeviceManager::getInstance().GetDevice()->CreateBuffer(&bd, &sd, _mVertexBuffer.GetAddressOf());

	assert(_mVertexBuffer);

	mVertexCount = static_cast<uint8_t>(vertex.size());

	return true;
}

bool Mesh::CreateUnevenness(const MeshType::Unevenness::DESC& desc)
{
	mType = Type::Unevenness;

	// 頂点・インデックス情報定義
	MeshType::Unevenness::DATA data = MeshType::Unevenness::Create(desc);

	if (data.vertices.empty() || data.indices.empty()) {
		return false;
	}

	// 頂点バッファ生成
	D3D11_BUFFER_DESC vertexDesc{};
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.ByteWidth = sizeof(Element::VERTEX3D) * static_cast<UINT>(data.vertices.size());
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexData{};
	vertexData.pSysMem = data.vertices.data();

	D3D11::DeviceManager::getInstance().GetDevice()->CreateBuffer(&vertexDesc, &vertexData, _mVertexBuffer.GetAddressOf());
	assert(_mVertexBuffer);

	// インデックスバッファ生成
	D3D11_BUFFER_DESC indexDesc{};
	indexDesc.Usage = D3D11_USAGE_DEFAULT;
	indexDesc.ByteWidth = sizeof(uint32_t) * static_cast<UINT>(data.indices.size());
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA indexData{};
	indexData.pSysMem = data.indices.data();

	D3D11::DeviceManager::getInstance().GetDevice()->CreateBuffer(&indexDesc, &indexData, _mIndexBuffer.GetAddressOf());
	assert(_mIndexBuffer);

	// 描画情報を保持
	mVertexCount = static_cast<uint32_t>(data.vertices.size());
	mIndexCount = static_cast<uint32_t>(data.indices.size());

	mIndexed = true;

	return true;
}

void Mesh::Bind() const
{
	ID3D11Buffer* vertexBuffer = _mVertexBuffer.Get();

	// 頂点バッファ設定
	UINT stride = sizeof(Element::VERTEX3D);
	UINT offset = 0;
	D3D11::DeviceManager::getInstance().GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	if (mIndexed) {
		D3D11::DeviceManager::getInstance().GetContext()->IASetIndexBuffer(_mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	// プリミティブトポロジ設定
	switch (mType) {
	case Type::Plane:
		D3D11::DeviceManager::getInstance().GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		break;

	case Type::Unevenness:
		D3D11::DeviceManager::getInstance().GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	}

}

void Mesh::Draw() const
{
	// 描画命令発行
	if (mIndexed) {
		D3D11::DeviceManager::getInstance().GetContext()->DrawIndexed(mIndexCount, 0, 0);
	}
	else {
		D3D11::DeviceManager::getInstance().GetContext()->Draw(mVertexCount, 0);
	}
}
