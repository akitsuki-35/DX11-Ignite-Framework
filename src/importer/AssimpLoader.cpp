/*============================================================
*	@file	 : AssimpLoader.cpp
*	@brief	 : assimpによるモデルロード
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/02
*	@updated : 2026/10/05
*============================================================*/
#include "AssimpLoader.h"
#include "DeviceManager.h"
#include "TextureManager.h"
#include "AnimationManager.h"
#include "Model.h"
#include "Texture.h"
#include "Animation.h"
#include "FileUtility.h"
#include <DirectXTex/DirectXTex.h>

// assimp関連
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#include "assimp/config.h"

using namespace Element;
using namespace DirectX;

// 補助関数宣言
namespace {
	XMFLOAT4X4 convertMatrix(const aiMatrix4x4& m) {
		XMFLOAT4X4 out {
			m.a1, m.b1, m.c1, m.d1,
			m.a2, m.b2, m.c2, m.d2,
			m.a3, m.b3, m.c3, m.d3,
			m.a4, m.b4, m.c4, m.d4
		};
		return out;
	}

	const aiScene* generateScene(Assimp::Importer& importer, const std::string& path) {
		// FBXのPivotノードをAssimp補助ノードとして展開しない
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

		// モデルロード
		const aiScene* scene = importer.ReadFile(
			path,
			aiProcess_Triangulate |
			aiProcess_ConvertToLeftHanded |
			aiProcess_JoinIdenticalVertices |
			aiProcess_GenSmoothNormals |
			aiProcess_LimitBoneWeights
		);

		return scene;
	}
}

/*--------------------------------------------------
	デバッグ用関数 プロトタイプ宣言
----------------------------------------------------*/
#ifndef NDEBUG
namespace AssimpDebug {
	void printMeshCount(const aiScene* scene);
	void printVertexCount(const aiMesh* mesh);
	void printVertexElement(Element::VERTEX3D& vertex, int v);
	void printIndexElement(const aiFace& face, int f);
	void printConvertedIndex(const std::vector<uint32_t>& indices,
		uint32_t startIndex, uint32_t indexNum);
	void printTextureElement(const aiTexture* tex, unsigned int index);
	void printAnimationElement(const aiScene* scene);
}
#endif

bool AssimpLoader::GenerateModel(Model& model, const std::string& path, const bool& isAnimLoad)
{
	mTextureMap.clear();

	Assimp::Importer importer{};

	const aiScene* scene = generateScene(importer, path);

	// 読み込み失敗時はreturn
	if (!scene || !scene->mRootNode) {
		OutputDebugStringA(importer.GetErrorString());
		return false;
	}

	// ノード読み込み
	if (!loadNodes(scene->mRootNode, model.mSkeleton, -1)) {
		return false;
	}

	// ボーン読み込み
	if (!loadBones(scene, model.mSkeleton)) {
		return false;
	}

	// グローバル逆行列作成
	aiMatrix4x4 inverseRoot = scene->mRootNode->mTransformation.Inverse();
	model.mSkeleton.SetGlobalInverse(convertMatrix(inverseRoot));

	// スキニング行列作成
	model.mSkeleton.ToBindPose();
	model.mSkeleton.Update();

	// メッシュ読み込み
	if (!loadMeshes(scene, model, model.mSkeleton)) {
		return false;
	}

	// 埋め込みアニメーション読み込み
	if (isAnimLoad) {
		if (scene->mAnimations) {
			if (!AiAnimationLoader::loadAnimations(scene, Utility::File::getFileName(path), true)) {
				return false;
			}
		}
	}

	// 埋め込みテクスチャ読み込み
	if (!loadTextures(scene, model)) {
		return false;
	}

	// マテリアル読み込み
	loadMaterials(scene, model, path);

	mTextureMap.clear();

	return true;
}

bool AssimpLoader::LoadFBXAnimation(const std::string& keyName, const std::string& filePath)
{
	Assimp::Importer importer{};

	const aiScene* scene = generateScene(importer, filePath);

	// 読み込み失敗時はreturn
	if (!scene || !scene->mRootNode) {
		OutputDebugStringA(importer.GetErrorString());
		return false;
	}

	if (scene->mAnimations) {
		if (!AiAnimationLoader::loadAnimations(scene, keyName)) {
			return false;
		}
	}

	return true;
}

bool AssimpLoader::loadNodes(const aiNode* node, Skeleton& skeleton, int parentIndex)
{
	if (!node) {
		return false;
	}

	// ノード生成
	Skeleton::NODE newNode{};

	newNode.Name = node->mName.C_Str();
	newNode.ParentIndex = parentIndex;

	// ローカル行列作成
	newNode.BindLocal = convertMatrix(node->mTransformation);

	// LocalとGlobal初期化
	newNode.Local = newNode.BindLocal;
	newNode.Global = newNode.BindLocal;

	int nodeIndex = skeleton.AddNode(newNode);

	// 親子関係を登録
	for (uint32_t i = 0; i < node->mNumChildren; ++i) {
		if (!loadNodes(node->mChildren[i], skeleton, nodeIndex)) {
			return false;
		}
	}

	return true;
}

bool AssimpLoader::loadBones(const aiScene* scene, Skeleton& skeleton)
{
	if (!scene) {
		return false;
	}

	// ボーン登録
	for (uint32_t i = 0; i < scene->mNumMeshes; ++i) {
		const aiMesh* mesh = scene->mMeshes[i];
		if (!mesh) continue;

		for (uint32_t j = 0; j < mesh->mNumBones; ++j) {
			const aiBone* bone = mesh->mBones[j];
			if (!bone) continue;

			std::string boneName = bone->mName.C_Str();

			int boneIndex = skeleton.FindBone(boneName);
			if (boneIndex >= 0) continue;

			if (skeleton.GetBoneCount() >= Element::MAX_BONE) {
				return false;
			}

			int nodeIndex = skeleton.FindNode(boneName);

			if (nodeIndex < 0) {
				return false;
			}

			Skeleton::BONE newBone{};
			newBone.Name = boneName;
			newBone.NodeIndex = nodeIndex;
			newBone.Offset = convertMatrix(bone->mOffsetMatrix);

			skeleton.AddBone(newBone);
		}
	}

	return true;
}

bool AssimpLoader::loadMeshes(const aiScene* scene, Model& model, const Skeleton& skeleton)
{
	// 全メッシュを処理
	for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {

		std::vector<VERTEX3D> vertices{};
		std::vector<uint32_t> indices{};

		aiMesh* mesh = scene->mMeshes[m];

		ModelMesh modelMesh{};

		// 現在メッシュのインデックス開始位置取得
		uint32_t startIndex = static_cast<uint32_t>(indices.size());

		/*--------------------------------------------------
			頂点データ作成
		----------------------------------------------------*/
		for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
			// aiMeshをVERTEX3Dへ変換
			vertices.push_back(convertVertex(mesh, v));
		}

		/*--------------------------------------------------
			頂点ウェイト取得
		----------------------------------------------------*/
		for (UINT boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {
			const aiBone* bone = mesh->mBones[boneIndex];

			// ボーンインデックス取得
			int skeletonIndex = skeleton.FindBone(bone->mName.C_Str());

			if (skeletonIndex < 0) {
				continue;
			}

			assert(skeletonIndex < Element::MAX_BONE);

			// ウェイト情報取得
			for (UINT weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				UINT vertexId = bone->mWeights[weightIndex].mVertexId;

				float weight = bone->mWeights[weightIndex].mWeight;

				if (vertexId >= vertices.size()) continue;

				Element::VERTEX3D& vertex = vertices[vertexId];

				// BoneWeightsとBoneIndicesを登録
				for (int slot = 0; slot < 4; ++slot) {
					if (vertex.BoneWeights[slot] == 0.0f) {
						vertex.BoneIndices[slot] = static_cast<uint32_t>(skeletonIndex);

						vertex.BoneWeights[slot] = weight;

						break;
					}
				}
			}
		}

		// ウェイト正規化
		for (auto& vertex : vertices)
		{
			float total =
				vertex.BoneWeights[0] +
				vertex.BoneWeights[1] +
				vertex.BoneWeights[2] +
				vertex.BoneWeights[3];

			if (total > 0.0f)
			{
				for (int i = 0; i < 4; ++i)
				{
					vertex.BoneWeights[i] /= total;
				}
			}
		}

		/*--------------------------------------------------
			インデックス取得
		----------------------------------------------------*/
		for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
			const aiFace& face = mesh->mFaces[f];

			for (unsigned int i = 0; i < face.mNumIndices; ++i) {
				indices.push_back(face.mIndices[i]);
			}
		}

		// メッシュインデックス計算
		uint32_t indexNum = static_cast<uint32_t>(indices.size()) - startIndex;

		// サブセット生成
		modelMesh.subsets.push_back(createSubset(mesh, startIndex, indexNum));

		// モデル用メッシュ作成
		if (!modelMesh.Create(vertices, indices))
			return false;

		// モデルにメッシュを登録
		model.AddMesh(std::move(modelMesh));
	}

	return true;
}

Element::VERTEX3D AssimpLoader::convertVertex(const aiMesh* mesh, int v)
{
	// 頂点データ取得
	VERTEX3D vertex{};

	// 座標取得
	vertex.Position =
	{
		mesh->mVertices[v].x,
		mesh->mVertices[v].y,
		mesh->mVertices[v].z
	};

	// 法線
	if (mesh->HasNormals())
	{
		vertex.Normal =
		{
			mesh->mNormals[v].x,
			mesh->mNormals[v].y,
			mesh->mNormals[v].z
		};
	}

	// UV
	if (mesh->HasTextureCoords(0))
	{
		vertex.TexCoord = {
			mesh->mTextureCoords[0][v].x,
			mesh->mTextureCoords[0][v].y
		};
	}

	// カラー
	if (mesh->HasVertexColors(0))
	{
		vertex.Diffuse = {
			mesh->mColors[0][v].r,
			mesh->mColors[0][v].g,
			mesh->mColors[0][v].b,
			mesh->mColors[0][v].a
		};
	}
	else
	{
		// 頂点カラーなしの場合は1.0に
		vertex.Diffuse = {
			1.0f,
			1.0f,
			1.0f,
			1.0f
		};
	}

	// ウェイト情報初期化
	vertex.BoneIndices[0] = 0;
	vertex.BoneIndices[1] = 0;
	vertex.BoneIndices[2] = 0;
	vertex.BoneIndices[3] = 0;

	vertex.BoneWeights[0] = 0.0f;
	vertex.BoneWeights[1] = 0.0f;
	vertex.BoneWeights[2] = 0.0f;
	vertex.BoneWeights[3] = 0.0f;

	return vertex;
}

ModelMesh::SUBSET AssimpLoader::createSubset(const aiMesh* mesh, uint32_t startIndex, uint32_t indexNum)
{
	// サブセット生成
	ModelMesh::SUBSET subset{};

	subset.StartIndex = startIndex;
	subset.IndexNum = indexNum;
	subset.MaterialIndex = mesh->mMaterialIndex;

	return subset;
}

bool AssimpLoader::loadTextures(const aiScene* scene, Model& model)
{
	// テクスチャ取得
	model.mTextures.reserve(scene->mNumTextures);

	for (unsigned int i = 0; i < scene->mNumTextures; ++i) {
		const aiTexture* tex = scene->mTextures[i];

		// キャッシュが存在すれば返す
		auto it = mTextureMap.find(tex->mFilename.C_Str());

		if (it != mTextureMap.end()) {
			model.mTextures.emplace_back(it->second);
		}

		// テクスチャ生成;
		auto texture = std::make_unique<Texture>();

		TexMetadata metadata{};
		ScratchImage image{};

		// テクスチャ読み込み
		HRESULT hr = LoadFromWICMemory(reinterpret_cast<const uint8_t*>(tex->pcData),
			static_cast<size_t>(tex->mWidth), WIC_FLAGS_NONE, &metadata, image);
		if (FAILED(hr)) return false;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;

		hr = CreateShaderResourceView(
			D3D11::DeviceManager::getInstance().GetDevice(),
			image.GetImages(), image.GetImageCount(), metadata, srv.GetAddressOf());
		if (FAILED(hr)) return false;

		// サイズとSRVを登録
		texture->mSize = { static_cast<UINT>(metadata.width), static_cast<UINT>(metadata.height) };
		texture->_mSRV = srv;

		// モデルへ登録
		Texture* ptr = texture.get();
		model.mTextures.push_back(std::move(texture));

		// 検索用マップへ登録
		mTextureMap.emplace(tex->mFilename.C_Str(), ptr);
	}

	return true;
}

void AssimpLoader::loadMaterials(const aiScene* scene, Model& model, const std::string& modelPath)
{
	// マテリアル取得
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
		Model::MATERIAL material{};

		aiMaterial* aiMat = scene->mMaterials[i];

		// テクスチャ取得
		aiString path{};

		if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {

			if (scene->mNumTextures > 0) {
				// 埋め込みテクスチャ取得
				auto it = mTextureMap.find(path.C_Str());

				if (it != mTextureMap.end()) {
					material._Texture = it->second;
					material.Material.TextureEnable = true;
				}
			}
			else {
				// mtlファイルからテクスチャ取得
				std::filesystem::path dir = Utility::File::getDirectoryPath(modelPath.c_str());

				// mtlファイル登録テクスチャ探索用パス
				std::filesystem::path mtlTexPath = dir / path.C_Str();

				// mtlファイル登録テクスチャをロード
				material._Texture = TextureManager::getInstance().Load(mtlTexPath.string().c_str(), false);

				if (!material._Texture) {
					return;
				}
				else {
					material.Material.TextureEnable = true;
				}
			}
		}

		// カラー取得
		aiColor4D color{};

		if (AI_SUCCESS == aiGetMaterialColor(aiMat, AI_MATKEY_COLOR_DIFFUSE, &color)) {
			material.Material.Diffuse = {
				color.r,
				color.g,
				color.b,
				color.a
			};
		}

		// マテリアル登録
		model.mMaterials.push_back(material);
	}
}

/*--------------------------------------------------
	アニメーションロード
----------------------------------------------------*/
bool AssimpLoader::AiAnimationLoader::loadAnimations(const aiScene* scene, const std::string& key, const bool isPrefix)
{
	// アニメーションなしの場合は無視
	if (scene->mNumAnimations == 0) {
		return false;
	}

	// シーンのアニメーション取得
	for (UINT i = 0; i < scene->mNumAnimations; i++) {
		auto animation = std::make_unique<Animation>();

		// 単一アニメーション取得
		loadAnimationClip(scene, *animation, i);

		// Manager登録処理
		std::string name{};
		if (isPrefix) {
			// 埋め込みアニメーションの場合はファイル名を接頭辞として使用
			name = key + "_" + std::to_string(i);
		}
		else {
			// 外部から明示的に読み込んだ場合は引数をそのままキー名にする
			name = key;
		}
		
		AnimationManager::getInstance().Register(name, std::move(animation));
	}

	return true;
}

bool AssimpLoader::AiAnimationLoader::loadAnimationClip(const aiScene* scene, Animation& animation, UINT index)
{
	// アニメーションなしの場合は無視
	if (scene->mNumAnimations == 0) {
		return false;
	}

	// 指定インデックスのアニメーションを取得
	const aiAnimation* aiAnim = scene->mAnimations[index];

	/*--------------------------------------------------
		アニメーション全体情報登録
	----------------------------------------------------*/
	// アニメーション総再生時間
	animation.mDuration = aiAnim->mDuration;

	// 1秒あたりのTick数
	animation.mTicksPerSecond = aiAnim->mTicksPerSecond != 0 ? aiAnim->mTicksPerSecond : 25.0;

	// ボーンごとのアニメーションチャンネル取得
	for (UINT channelIndex = 0; channelIndex < aiAnim->mNumChannels; channelIndex++) {
		const aiNodeAnim* aiChannel = aiAnim->mChannels[channelIndex];

		// ノード名取得
		std::string nodeName = aiChannel->mNodeName.C_Str();

		// アニメーションチャンネル作成
		Animation::CHANNEL channel{};
		channel.NodeName = nodeName;
		channel.NodeIndex = -1;

		/*--------------------------------------------------
			移動キー
		----------------------------------------------------*/
		for (UINT i = 0; i < aiChannel->mNumPositionKeys; i++) {
			const aiVectorKey& key = aiChannel->mPositionKeys[i];

			Animation::KEY_POSITION position{};

			position.Time = static_cast<double>(key.mTime);
			position.Position = { key.mValue.x, key.mValue.y, key.mValue.z };

			channel.Positions.push_back(position);
		}

		/*--------------------------------------------------
			回転キー
		----------------------------------------------------*/
		for (UINT i = 0; i < aiChannel->mNumRotationKeys; i++) {
			const aiQuatKey& key = aiChannel->mRotationKeys[i];

			Animation::KEY_ROTATION rotation{};

			rotation.Time = static_cast<double>(key.mTime);
			rotation.Rotation = { key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w };


			channel.Rotations.push_back(rotation);
		}

		/*--------------------------------------------------
			拡大縮小キー
		----------------------------------------------------*/
		for (UINT i = 0; i < aiChannel->mNumScalingKeys; i++) {
			const aiVectorKey& key = aiChannel->mScalingKeys[i];

			Animation::KEY_SCALE scale{};

			scale.Time = static_cast<double>(key.mTime);
			scale.Scale = { key.mValue.x, key.mValue.y, key.mValue.z };

			channel.Scales.push_back(scale);
		}

		// チャンネルをアニメーションに登録
		animation.AddChannel(channel);
	}

	return true;
}

/*--------------------------------------------------
	デバッグ用関数
----------------------------------------------------*/
#ifndef NDEBUG
void AssimpDebug::printMeshCount(const aiScene* scene)
{
#ifndef NDEBUG
	OutputDebugStringA(
		("----------printMeshCount----------\n"
			"Meshs : " + std::to_string(scene->mNumMeshes) + "\n").c_str());
#endif
}

void AssimpDebug::printVertexCount(const aiMesh* mesh)
{
#ifndef NDEBUG
	OutputDebugStringA(
		("----------printVertexCount----------\n"
			"Vertices : " +
			std::to_string(mesh->mNumVertices) + "\n").c_str());
#endif
}

void AssimpDebug::printVertexElement(Element::VERTEX3D& vertex, int v)
{
#ifndef NDEBUG
	if (v < 5)
	{
		std::string log =
			"----------printVertexElement----------\n"
			"Vertex[" + std::to_string(v) + "]\n" +
			" Pos : " +
			std::to_string(vertex.Position.x) + ", " +
			std::to_string(vertex.Position.y) + ", " +
			std::to_string(vertex.Position.z) + "\n" +
			" Normal : " +
			std::to_string(vertex.Normal.x) + ", " +
			std::to_string(vertex.Normal.y) + ", " +
			std::to_string(vertex.Normal.z) + "\n" +
			" UV : " +
			std::to_string(vertex.TexCoord.x) + ", " +
			std::to_string(vertex.TexCoord.y) + "\n" +
			" Diffuse : " +
			std::to_string(vertex.Diffuse.x) + ", " +
			std::to_string(vertex.Diffuse.y) + ", " +
			std::to_string(vertex.Diffuse.z) + ", " +
			std::to_string(vertex.Diffuse.w) + "\n";

		OutputDebugStringA(log.c_str());
	}
#endif
}

void AssimpDebug::printIndexElement(const aiFace& face, int f)
{
#ifndef NDEBUG
	std::string log =
		"----------printIndexElement----------\n"
		"Face[" + std::to_string(f) + "] : " +
		std::to_string(face.mIndices[0]) + ", " +
		std::to_string(face.mIndices[1]) + ", " +
		std::to_string(face.mIndices[2]) +
		"\n";

	OutputDebugStringA(log.c_str());
#endif
}

void AssimpDebug::printConvertedIndex(const std::vector<uint32_t>& indices, uint32_t startIndex, uint32_t indexNum)
{
#ifndef NDEBUG
	std::string log = "----------printConvertedIndex----------\n";

	for (uint32_t i = startIndex; i < startIndex + indexNum; i += 3)
	{
		log +=
			"Triangle[" +
			std::to_string((i - startIndex) / 3) +
			"] : " +
			std::to_string(indices[i]) + ", " +
			std::to_string(indices[i + 1]) + ", " +
			std::to_string(indices[i + 2]) +
			"\n";
	}

	OutputDebugStringA(log.c_str());
#endif
}

void AssimpDebug::printTextureElement(const aiTexture* tex, unsigned int index)
{
#ifndef NDEBUG

	std::string log = "----------printTextureElement----------\n"
		"Texture[" + std::to_string(index) + "]\n" +
		" Filename : " + std::string(tex->mFilename.C_Str()) + "\n" +
		" Width : " + std::to_string(tex->mWidth) + "\n" +
		" Height : " + std::to_string(tex->mHeight) + "\n";

	OutputDebugStringA(log.c_str());

#endif
}

void AssimpDebug::printAnimationElement(const aiScene* scene)
{
#ifndef NDEBUG

	std::string log = "----------printAnimationElement----------\n";

	// シーン情報取得
	log += std::format("Scene\n" " Meshes:{}\n" " Materials:{}\n" " Animations:{}\n",
		scene->mNumMeshes, scene->mNumMaterials, scene->mNumAnimations).c_str();

	// アニメーション情報取得
	const aiAnimation* anim = scene->mAnimations[0];

	log += std::format("Animation\n" "Name:{}\n" "Duration:{}\n" "TicksPerSecond:{}\n" "Channels:{}\n",
		anim->mName.C_Str(), anim->mDuration, anim->mTicksPerSecond, anim->mNumChannels).c_str();

	// チャンネル情報取得
	for (UINT i = 0; i < anim->mNumChannels; i++)
	{
		auto channel = anim->mChannels[i];

		log += std::format("Channel:{}\n", channel->mNodeName.C_Str()).c_str();
	}

	// キー情報取得
	const aiNodeAnim* channel = anim->mChannels[0];

	log += std::format("PositionKeys:{}\n" "RotationKeys:{}\n" "ScaleKeys:{}\n",
		channel->mNumPositionKeys, channel->mNumRotationKeys, channel->mNumScalingKeys).c_str();

	for (UINT i = 0; i < channel->mNumRotationKeys; i++)
	{
		auto& key = channel->mRotationKeys[i];

		log += std::format("Time:{}\n" "Quat:{} {} {} {}\n",
			key.mTime, key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w
		).c_str();
	}

	OutputDebugStringA(log.c_str());

#endif
}
#endif