/*============================================================
*	@file	 : MeshTypes.cpp
*	@brief	 : メッシュタイプ定義
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/07/26
*	@updated : 2026/11/11
*============================================================*/
#include "MeshTypes.h"
#include "Vector2.h"
#include "Vector3.h"
#include <cmath>

using namespace DirectX;

/*--------------------------------------------------
    定数・構造体定義
----------------------------------------------------*/
namespace {
    // 標準となる頂点座標を定義
    constexpr std::array<Vector3, 4> BASE_VERTEX
    {
        Vector3{ -0.5f,  0.5f, 0.0f },
        Vector3{  0.5f,  0.5f, 0.0f },
        Vector3{ -0.5f, -0.5f, 0.0f },
        Vector3{  0.5f, -0.5f, 0.0f }
    };

    // UV座標定義
    constexpr std::array<Vector2, 4> UV
    {
        Vector2{ 0.0f, 0.0f },
        Vector2{ 1.0f, 0.0f },
        Vector2{ 0.0f, 1.0f },
        Vector2{ 1.0f, 1.0f }
    };

    // 向き情報構造体
    struct AXIS
    {
        // 代入軸情報
        uint8_t x;
        uint8_t y;
        uint8_t z;

        Vector3 normal; // 法線
    };

    // ピボット座標テーブル
    inline constexpr std::array<Vector2, static_cast<size_t>(MeshType::Plane::Pivot::Count)> OFFSET_TABLE
    {
                // X      Y 
        Vector2{  0.0f,  0.0f }, // 0.Center
        Vector2{  0.0f, -0.5f }, // 1.CenterTop
        Vector2{  0.0f,  0.5f }, // 2.CenterBottom
        Vector2{  0.5f, -0.5f }, // 3.LeftTop
        Vector2{  0.5f,  0.5f }, // 4.LeftBottom
        Vector2{ -0.5f, -0.5f }, // 5.RightTop
        Vector2{ -0.5f,  0.5f }  // 6.RightBottom
    };

    // 向き情報テーブル
    inline constexpr std::array<AXIS, static_cast<size_t>(MeshType::Plane::Axis::Count)> AXIS_TABLE
    {
        //    X  Y  Z                   法線
        AXIS{ 0, 1, 2 , Vector3{  0.0f, 0.0f ,1.0f }}, // 0.XY
        AXIS{ 0, 2, 1 , Vector3{  0.0f, 1.0f ,0.0f }}, // 1.XZ
        AXIS{ 1, 2, 0 , Vector3{  1.0f, 0.0f ,0.0f }}  // 2.YZ
    };

    /*--------------------------------------------------
        向き変換
    ----------------------------------------------------*/
    Vector3 ConvertAxis(const Vector3& position, const AXIS& axisInfo)
    {
        Vector3 newPosition{};

        std::array <float, 3>v{};

        // 座標代入
        v[axisInfo.x] = position.x;
        v[axisInfo.y] = position.y;
        v[axisInfo.z] = position.z;

        newPosition = { v[0], v[1], v[2] };

        return newPosition;
    }

    /*--------------------------------------------------
        頂点生成
    ----------------------------------------------------*/
    Element::VERTEX3D CreateVertex(size_t index, const MeshType::Plane::DESC& desc)
    {
        Element::VERTEX3D vertex{};

        // 基準座標で初期化
        Vector3 position = BASE_VERTEX[index];

        // ピボット補正
        const Vector2 offset = OFFSET_TABLE[static_cast<size_t>(desc.pivot)];
        Vector2 pos = { position.x, position.y };
        pos += offset;
        position = { pos.x, pos.y, position.z };

        // 向きを変換
        position = ConvertAxis(position, AXIS_TABLE[static_cast<size_t>(desc.axis)]);

        // 頂点パラメータ設定
        vertex.Position = position.ConvertToXMFLOAT3();
        vertex.Normal = AXIS_TABLE[static_cast<size_t>(desc.axis)].normal.ConvertToXMFLOAT3();
        vertex.Diffuse = XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f };
        vertex.TexCoord = UV[index].ConvertToXMFLOAT2();

        return vertex;
    }

    /*--------------------------------------------------
        高さ計算
    ----------------------------------------------------*/
    float CalculateUnevennessHeight(float x, float z, const MeshType::Unevenness::DESC& desc) {
        const float heightX = std::sin(x * desc.frequency) * desc.height;
        const float heightZ = std::cos(z * desc.frequency) * desc.height;

        return heightX + heightZ;
    }
}

std::array<Element::VERTEX3D, 4> MeshType::Plane::Create(const DESC& desc)
{
    std::array<Element::VERTEX3D, 4> vertices{};

    for (size_t i = 0; i < vertices.size(); i++) {
        vertices[i] = CreateVertex(i, desc);
    }

    return vertices;
}

MeshType::Unevenness::DATA MeshType::Unevenness::Create(const DESC& desc)
{
    DATA data{};

    // 頂点を生成できない場合はreturn
    if (desc.row < 2 || desc.column < 2) {
        return data;
    }

    if (desc.width <= 0.0f || desc.depth <= 0.0f) {
        return data;
    }

    // 頂点の間隔
    const float intervalX = desc.width / static_cast<float>(desc.column - 1);
    const float intervalZ = desc.depth / static_cast<float>(desc.row - 1);

    // 地形の開始座標
    const float startX = -desc.width * 0.5f;
    const float startZ = -desc.depth * 0.5f;

    // 頂点生成
    for (uint32_t r = 0; r < desc.row; ++r) {
        for (uint32_t c = 0; c < desc.column; ++c) {
            const float x = startX + static_cast<float>(c) * intervalX;
            const float z = startZ + static_cast<float>(r) * intervalZ;
            const float y = CalculateUnevennessHeight(x, z, desc);

            Element::VERTEX3D vertex{};
            vertex.Position = { x, y, z };
            vertex.Normal = { 0.0f, 1.0f, 0.0f };
            vertex.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
            vertex.TexCoord = { static_cast<float>(c) / static_cast<float>(desc.column - 1),
                static_cast<float>(r) / static_cast<float>(desc.row - 1) };

            data.vertices.push_back(vertex);
        }
    }

    // インデックス生成
    for (uint32_t r = 0; r < desc.row - 1; ++r) {
        for (uint32_t c = 0; c < desc.column - 1; ++c) {
            // 現在のマスを構成する4頂点を取得
            const uint32_t leftTop = r * desc.column + c;
            const uint32_t rightTop = leftTop + 1;
            const uint32_t leftBottom = leftTop + desc.column;
            const uint32_t rightBottom = leftBottom + 1;

            // インデックスを詰める
            data.indices.push_back(leftTop);
            data.indices.push_back(leftBottom);
            data.indices.push_back(rightTop);

            data.indices.push_back(rightTop);
            data.indices.push_back(leftBottom);
            data.indices.push_back(rightBottom);
        }
    }

    return data;
}

bool MeshType::Unevenness::GetHeight(const DESC& desc, float x, float z, float& height)
{
    // 地形として成立しないデータの場合はreturn
    if (desc.row < 2 || desc.column < 2) {
        return false;
    }

    if (desc.width <= 0.0f || desc.depth <= 0.0f) {
        return false;
    }

    const float halfWidth = desc.width * 0.5f;
    const float halfDepth = desc.depth * 0.5f;

    // 地形の範囲外
    if (x < -halfWidth || x > halfWidth || z < -halfDepth || z > halfDepth) {
        return false;
    }

    // 頂点の間隔
    const float intervalX = desc.width / static_cast<float>(desc.column - 1);
    const float intervalZ = desc.depth / static_cast<float>(desc.row - 1);

    // 地形左上から見た座標を算出
    const float localX = x + halfWidth;
    const float localZ = z + halfDepth;

    // 現在位置が含まれるマスを算出
    uint32_t column = static_cast<uint32_t>(localX / intervalX);
    uint32_t row = static_cast<uint32_t>(localZ / intervalZ);

    // 右端・下端対策
    if (column >= desc.column - 1) {
        column = desc.column - 2;
    }

    if (row >= desc.row - 1) {
        row = desc.row - 2;
    }

    // マスの左上座標
    const float cellX = -halfWidth + static_cast<float>(column) * intervalX;
    const float cellZ = -halfDepth + static_cast<float>(row) * intervalZ;

    // マス内での位置を0～1に変換
    const float rateX = (x - cellX) / intervalX;
    const float rateZ = (z - cellZ) / intervalZ;

    // 4頂点の高さを求める
    const float leftTop = CalculateUnevennessHeight(cellX, cellZ, desc);
    const float rightTop = CalculateUnevennessHeight(cellX + intervalX, cellZ, desc);
    const float leftBottom = CalculateUnevennessHeight(cellX, cellZ + intervalZ, desc);
    const float rightBottom = CalculateUnevennessHeight(cellX + intervalX, cellZ + intervalZ, desc);

    // 左上・左下・右上の三角形
    if (rateX + rateZ <= 1.0f) {
        height = leftTop + (rightTop - leftTop) * rateX + (leftBottom - leftTop) * rateZ;
    }
    // 右上・左下・右下の三角形
    else {
        const float rightRate = 1.0f - rateX;
        const float topRate = 1.0f - rateZ;
        height = rightBottom + (leftBottom - rightBottom) * rightRate + (rightTop - rightBottom) * topRate;
    }

    return true;
}
