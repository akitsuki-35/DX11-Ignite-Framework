/*============================================================
*	@file	 : AssimpLoader.h
*	@brief	 : assimpによるモデルロード
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/02
*	@updated : 2026/10/05
*============================================================*/
#pragma once

#include "ModelMesh.h"
#include "Elements.h"
#include <string>
#include <vector>
#include <unordered_map>

/*--------------------------------------------------
	前方宣言
----------------------------------------------------*/
class Model;
class Texture;
class Skeleton;
class Animation;
struct aiScene;
struct aiMesh;
struct aiNode;

/*============================================================
*	@class	: AssimpLoader
*	@brief	: assimpによるモデルロード
*============================================================*/
class AssimpLoader final
{
/*--------------------------------------------------
	Singleton用
----------------------------------------------------*/
public:
	static AssimpLoader& getInstance() {
		static AssimpLoader  instance;
		return instance;
	}

private:
	AssimpLoader() = default;
	AssimpLoader(const AssimpLoader&) = delete;

	AssimpLoader& operator=(const AssimpLoader&) = delete;
	AssimpLoader(AssimpLoader&&) = delete;

	AssimpLoader& operator=(AssimpLoader&&) = delete;
	~AssimpLoader() {};

/*--------------------------------------------------
	メンバ変数・メンバ関数
----------------------------------------------------*/
private:
	// 埋め込みテクスチャ検索用マップ
	std::unordered_map<std::string, Texture*> mTextureMap{};

public:
	// モデル生成
	bool GenerateModel(Model& model, const std::string& path);

	// FBXアニメーション読み込み
	bool LoadFBXAnimation(Animation& anim, const std::string& path);

private:
	// ノード取得
	bool loadNodes(const aiNode* node, Skeleton& skeleton, int parentIndex);

	// ボーン取得
	bool loadBones(const aiScene* scene, Skeleton& skeleton);

	// メッシュ生成
	bool loadMeshes(const aiScene* scene, Model& model, const Skeleton& skeleton);

	// 頂点データ取得
	Element::VERTEX3D convertVertex(const aiMesh* mesh, int v);

	// サブセット生成
	ModelMesh::SUBSET createSubset(const aiMesh* mesh, uint32_t startIndex, uint32_t indexNum);

	// 埋め込みテクスチャ取得
	bool loadTextures(const aiScene* scene, Model& model);

	// マテリアル取得
	void loadMaterials(const aiScene* scene, Model& model, const std::string& modelPath);

	/*============================================================
	*	@class	: AiAnimationLoader
	*	@brief	: アニメーション読み込み
	*============================================================*/
	class AiAnimationLoader
	{
		friend AssimpLoader;

	private:
		AiAnimationLoader() = delete;

		// アニメーション取得
		static bool loadAnimations(const aiScene* scene, const Skeleton& skeleton,
			const std::string& key, const bool isPrefix = false);

		// 単一アニメーション取得
		static bool loadAnimationClip(const aiScene* scene, const Skeleton& skeleton,
			Animation& animation, UINT index);
	};
};