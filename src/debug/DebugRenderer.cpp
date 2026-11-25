/*============================================================
*	@file	 : DebugRenderer.cpp
*	@brief	 : デバッグ用描画
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/25
*	@updated : 2026/11/25
*============================================================*/
#ifndef NDEBUG
#include "DebugRenderer.h"
#include "DeviceManager.h"
#include "BufferManager.h"
#include "GameObject.h"
#include <d3d11.h>

DebugRenderer::DebugRenderer(GameObject* owner)
    : Renderer(owner){}

DebugRenderer::~DebugRenderer() = default;

void DebugRenderer::Finalize()
{
    mVertices.clear();
    mVertexBuffer = nullptr;

    Renderer::Finalize();
}

void DebugRenderer::Draw() const
{
    if (!mEnable) {
        return;
    }

    if (!_mShader) {
        return;
    }

    if (mVertices.empty()) {
        return;
    }

    Renderer::Begin();
    Bind();

    // ワールド行列取得
    D3D11::BufferManager::getInstance().SetWorldMatrix(getWorldMatrix());
    
    // マテリアル設定
    Element::MATERIAL material{};
    material.Diffuse = mColor;
    material.TextureEnable = false;
    D3D11::BufferManager::getInstance().SetMaterial(material);

    // パラメータ設定
    D3D11::BufferManager::getInstance().SetParameter(mParameter);

    // 頂点バッファ更新
    D3D11_MAPPED_SUBRESOURCE mapped{};
    auto* context = D3D11::DeviceManager::getInstance().GetContext();
    context->Map(mVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, mVertices.data(), sizeof(Element::VERTEX3D) * mVertices.size());
    context->Unmap(mVertexBuffer.Get(), 0);

    // 頂点バッファ設定
    UINT stride = sizeof(Element::VERTEX3D);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);

    // プリミティブトポロジ設定
    D3D11::DeviceManager::getInstance().GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    // 描画命令発行
    D3D11::DeviceManager::getInstance().GetContext()->Draw(static_cast<UINT>(mVertices.size()), 0);

    Renderer::End();
}

DebugRenderer* DebugRenderer::CreateLine(const Vector3& start, const Vector3& end)
{
    Element::VERTEX3D vertex[2]{};

    vertex[0].Position = { start.x, start.y, start.z };
    vertex[0].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

    vertex[1].Position = { end.x, end.y, end.z };
    vertex[1].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

    mVertices.emplace_back(vertex[0]);
    mVertices.emplace_back(vertex[1]);

    // 必要なサイズの頂点バッファを確保
    createVertexBuffer(static_cast<UINT>(mVertices.size()));

    return this;
}

DebugRenderer* DebugRenderer::CreateBox(const Vector3& center, const Vector3& size)
{
    const Vector3 half = size * 0.5f;

    const Vector3 min = center - half;
    const Vector3 max = center + half;

    // ボックス生成
    const Vector3 vertex[8] {
        { min.x, min.y, min.z },
        { max.x, min.y, min.z },
        { max.x, max.y, min.z },
        { min.x, max.y, min.z },

        { min.x, min.y, max.z },
        { max.x, min.y, max.z },
        { max.x, max.y, max.z },
        { min.x, max.y, max.z }
    };

    // 手前
    CreateLine(vertex[0], vertex[1]);
    CreateLine(vertex[1], vertex[2]);
    CreateLine(vertex[2], vertex[3]);
    CreateLine(vertex[3], vertex[0]);

    // 奥
    CreateLine(vertex[4], vertex[5]);
    CreateLine(vertex[5], vertex[6]);
    CreateLine(vertex[6], vertex[7]);
    CreateLine(vertex[7], vertex[4]);

    // 手前と奥を接続
    CreateLine(vertex[0], vertex[4]);
    CreateLine(vertex[1], vertex[5]);
    CreateLine(vertex[2], vertex[6]);
    CreateLine(vertex[3], vertex[7]);

    // 必要なサイズの頂点バッファを確保
    createVertexBuffer(static_cast<UINT>(mVertices.size()));

    return this;
}

void DebugRenderer::Clear()
{
    mVertices.clear();
}

void DebugRenderer::createVertexBuffer(UINT vertices)
{
    if (vertices <= mVertexCapacity) {
        return;
    }

    // 頂点バッファ確保
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(Element::VERTEX3D) * vertices;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    mVertexBuffer.Reset();
    D3D11::DeviceManager::getInstance().GetDevice()->CreateBuffer(&bd, nullptr, mVertexBuffer.GetAddressOf());
    mVertexCapacity = vertices;
}

DirectX::XMMATRIX DebugRenderer::getWorldMatrix() const
{
    const Vector3 position = _mOwner->GetPosition() + mOffset;

    return DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}

#endif