/*============================================================
*	@file	 : Mesh.h
*	@brief	 : ポリゴンメッシュ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/07/24
*	@updated : 2026/11/05
*============================================================*/
#pragma once

#include "MeshTypes.h"
#include <wrl/client.h>
#include <d3d11.h>

/*============================================================
*	@class	: Mesh
*	@brief	: ポリゴンメッシュ
*============================================================*/
class Mesh
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	// メッシュ形状列挙体
	enum class Type : uint8_t
	{
		Plane,
		Unevenness
	};

private:
	// メッシュ形状
	Type mType{};

	// 頂点数
	uint32_t mVertexCount{};

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> _mVertexBuffer{};

	// インデックス数
	uint32_t mIndexCount{};

	// インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> _mIndexBuffer{};

	// インデックスを使用するか
	bool mIndexed{ false };

public:
	Mesh() = default;
	~Mesh() = default;

	// 板ポリゴン作成
	bool CreatePlane(MeshType::Plane::Pivot pivot, MeshType::Plane::Axis axis);

	// 凹凸地形作成
	bool CreateUnevenness(const MeshType::Unevenness::DESC& desc);

	void Bind() const;
	void Draw() const;

	// インスタンス描画
	void DrawInstanced(uint32_t instance) const;
};