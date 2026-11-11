/*============================================================
*	@file	 : UIStyle.cpp
*	@brief	 : UI表示スタイル設定
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/12
*	@updated : 2026/11/11
*============================================================*/
#include "UIStyle.h"
#include "Vector2.h"
#include "Vector3.h"
#include <d3d11.h>

using namespace DirectX;

/*--------------------------------------------------
    定数・構造体定義
----------------------------------------------------*/
namespace {

    // 標準となる頂点座標を定義
    constexpr std::array<Vector3, 4> BASE_VERTEX
    {
        Vector3{ -0.5f, -0.5f, 0.0f },
        Vector3{  0.5f, -0.5f, 0.0f },
        Vector3{ -0.5f,  0.5f, 0.0f },
        Vector3{  0.5f,  0.5f, 0.0f }
    };

    // UV座標定義
    constexpr std::array<Vector2, 4> UV
    {
        Vector2{ 0.0f, 0.0f },
        Vector2{ 1.0f, 0.0f },
        Vector2{ 0.0f, 1.0f },
        Vector2{ 1.0f, 1.0f }
    };

    // ピボット座標テーブル
    inline constexpr std::array<Vector2, static_cast<size_t>(UIStyle::Pivot::Count)> OFFSET_TABLE
    {
                // X      Y
        Vector2{  0.0f,  0.0f }, // 0.Center
        Vector2{  0.0f,  0.5f }, // 1.CenterTop
        Vector2{  0.0f, -0.5f }, // 2.CenterBottom
        Vector2{  0.5f,  0.5f }, // 3.LeftTop
        Vector2{  0.5f, -0.5f }, // 4.LeftBottom
        Vector2{ -0.5f,  0.5f }, // 5.RightTop
        Vector2{ -0.5f, -0.5f }  // 6.RightBottom
    };

    /*--------------------------------------------------
        頂点生成
    ----------------------------------------------------*/
    Element::VERTEX3D CreateVertex(size_t index, const UIStyle::Pivot& pivot)
    {
        Element::VERTEX3D vertex{};

        // 基準座標で初期化
        Vector3 position = BASE_VERTEX[index];

        // ピボット補正
        const Vector2 offset = OFFSET_TABLE[static_cast<size_t>(pivot)];
        Vector2 pos = { position.x, position.y };
        pos += offset;
        position = { pos.x, pos.y, position.z };

        // 頂点パラメータ設定
        vertex.Position = position.ConvertToXMFLOAT3();
        vertex.Normal = { 0.0f, 0.0f, -1.0f };
        vertex.Diffuse = XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f };
        vertex.TexCoord = UV[index].ConvertToXMFLOAT2();

        return vertex;
    }
}

std::array<Element::VERTEX3D, 4> UIStyle::Create(const Pivot& pivot)
{
    std::array<Element::VERTEX3D, 4> vertices{};

    for (size_t i = 0; i < vertices.size(); i++) {
        vertices[i] = CreateVertex(i, pivot);
    }

    return vertices;
}
