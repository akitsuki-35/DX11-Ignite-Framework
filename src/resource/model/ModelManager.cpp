/*============================================================
*	@file	 : ModelManager.cpp
*	@brief	 : モデルリソース管理
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/07/30
*	@updated : 2026/10/05
*============================================================*/
#include "ModelManager.h"
#include "AssimpLoader.h"
#include "FileUtility.h"
#include "Model.h"

Model* ModelManager::Load(const char* modelPath, const bool& isAnimLoad)
{
	// キャッシュ取得用にパスを正規化
	std::string key = Utility::File::normalizePath(modelPath);

	// キャッシュが存在すれば返す
	auto it = mModels.find(key);

	if (it != mModels.end()) {
		return it->second.get();
	}

	// モデル生成
	std::unique_ptr<Model> model = std::make_unique<Model>();

	// Loaderからモデルをインポート
	if (!AssimpLoader::getInstance().GenerateModel(*model, key, isAnimLoad)) {
		return nullptr;
	}

	Model* m = model.get();
	
	// モデル登録
	mModels.emplace(key, std::move(model));

	return m;
}

void ModelManager::Clear()
{
	mModels.clear();
}