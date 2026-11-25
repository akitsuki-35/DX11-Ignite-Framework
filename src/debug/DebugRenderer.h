/*============================================================
*	@file	 : DebugRenderer.h
*	@brief	 : デバッグ用描画
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/25
*	@updated : 2026/11/25
*============================================================*/
#pragma once
#ifndef NDEBUG

#include "Renderer.h"
#include "Vector3.h"
#include "Elements.h"
#include <string>
#include <vector>
#include <wrl/client.h>

/*------------------------------------------------------------
	前方宣言
------------------------------------------------------------*/
struct ID3D11Buffer;
class Shader;

/*============================================================
*	@class	: DebugRenderer
*	@brief	: デバッグ用描画
*============================================================*/
class DebugRenderer final : public Renderer
{
private:
    // 頂点
    std::vector<Element::VERTEX3D> mVertices{};

    // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer{};

    // 確保中の頂点数
    UINT mVertexCapacity{};

    // 描画座標オフセット
    Vector3 mOffset{};

public:
    DebugRenderer(GameObject* owner);

    ~DebugRenderer() override;;

    void Finalize() override;
    void Draw() const override;

    // ラインを追加
    DebugRenderer* CreateLine(const Vector3& start, const Vector3& end);

    // ボックスを追加
    DebugRenderer* CreateBox(const Vector3& center, const Vector3& size);

    // 頂点をクリア
    void Clear();

    // オフセット設定
    void SetOffset(const Vector3& offset) { mOffset = offset; }

private:
    // 必要に応じて頂点バッファを確保
    void createVertexBuffer(UINT vertices);

    DirectX::XMMATRIX getWorldMatrix() const override;
};

#endif