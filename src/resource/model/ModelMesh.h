/*============================================================
*	@file	 : ModelMesh.h
*	@brief	 : モデル用メッシュ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/01
*	@updated : 2026/11/05
*============================================================*/
#pragma once

#include "Elements.h"
#include <vector>
#include <wrl/client.h>
#include <d3d11.h>

/*============================================================
*	@class	: ModelMesh
*	@brief	: モデル用メッシュ
*============================================================*/
class ModelMesh
{
	friend class AssimpLoader;
	friend class ModelRenderer;

	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private:

	// サブセット
	struct SUBSET
	{
		uint32_t StartIndex{};
		uint32_t IndexNum{};
		uint32_t MaterialIndex{};
	};

	// バッファ生成用一時データ
	std::vector<Element::VERTEX3D> mVertices{};
	std::vector<uint32_t> mIndices{};

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> _mVertexBuffer{};

	// インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> _mIndexBuffer{};

	// サブセット
	std::vector<SUBSET> mSubsets{};

public:
	ModelMesh() = default;
	~ModelMesh() = default;

	// モデルメッシュ生成
	bool Create(const std::vector<Element::VERTEX3D>& vertices,
		const std::vector<uint32_t>& indices);

	void Bind() const;
	void Draw(const SUBSET& subset) const;

	// インスタンス描画
	void DrawInstanced(const SUBSET& subset, uint32_t instance) const;

private:
	// 頂点バッファ生成
	bool generateVertexBuffer();

	// インデックスバッファ生成
	bool generateIndexBuffer();
};