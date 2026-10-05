/*============================================================
*	@file	 : ModelManager.h
*	@brief	 : モデルリソース管理
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/07/30
*	@updated : 2026/10/05
*============================================================*/
#pragma once

#include "Model.h"
#include <string>
#include <memory>
#include <unordered_map>

/*============================================================
*	@class	: ModelManager
*	@brief	: モデルのロード・管理
*============================================================*/
class ModelManager final
{
/*--------------------------------------------------
	Singleton用
----------------------------------------------------*/
public:
	static ModelManager& getInstance() {
		static ModelManager  instance;
		return instance;
	}

private:
	ModelManager() = default;
	ModelManager(const ModelManager&) = delete;

	ModelManager& operator=(const ModelManager&) = delete;
	ModelManager(ModelManager&&) = delete;

	ModelManager& operator=(ModelManager&&) = delete;
	~ModelManager() {};

/*--------------------------------------------------
	メンバ変数・メンバ関数
----------------------------------------------------*/
private:
	// モデルコンテナ
	std::unordered_map <std::string, std::unique_ptr<Model>> mModels{};

public:
	// ロード
	Model* Load(const char* modelPath, const bool& isAnimLoad);

	// クリア
	void Clear();
};