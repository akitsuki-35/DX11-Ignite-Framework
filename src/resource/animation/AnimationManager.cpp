/*============================================================
*	@file	 : AnimationManager.cpp
*	@brief	 : アニメーションクリップ管理
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/13
*	@updated : 2026/10/05
*============================================================*/
#include "AnimationManager.h"
#include "AssimpLoader.h"
#include "FileUtility.h"
#include "Animation.h"

Animation* AnimationManager::Load(const char* animPath)
{
	// キャッシュ取得用にパスを正規化
	std::string key = Utility::File::normalizePath(animPath);

	// キャッシュが存在すれば返す
	auto it = mAnimations.find(key);

	if (it != mAnimations.end()) {
		return it->second.get();
	}

	// アニメーション生成
	std::unique_ptr<Animation> anim = std::make_unique<Animation>();

	//// アニメーションをインポート
	//if (!AssimpLoader::AiAnimationLoader::loadAnimationClip(*anim, key)) {
	//	return nullptr;
	//}

	Animation* a = anim.get();

	// アニメーション登録
	mAnimations.emplace(key, std::move(anim));

	return a;
}

Animation* AnimationManager::Get(const std::string& keyName)
{
	// アニメーション取得
	auto it = mAnimations.find(keyName);

	if (it != mAnimations.end()) {
		return it->second.get();
	}

	return nullptr;
}

Animation* AnimationManager::Register(const std::string& keyName, std::unique_ptr<Animation> animation)
{
	// 登録済みならreturn
	if (mAnimations.contains(keyName)) {
		return mAnimations[keyName].get();
	}

	// アニメーション登録
	mAnimations.emplace(keyName, std::move(animation));

	return mAnimations[keyName].get();
}

void AnimationManager::Clear()
{
	mAnimations.clear();
}