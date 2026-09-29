/*============================================================
*	@file	 : Scene.h
*	@brief	 : シーン基底クラス
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2025/09/17
*	@updated : 2026/09/16
*============================================================*/
#pragma once

#include "GameObject.h"
#include <list>
#include <vector>
#include <memory>

/*============================================================
*	@class	: Scene
*	@brief	: シーン基底クラス
*============================================================*/
class Scene
{
protected:
	// ゲームオブジェクト配列
	static inline std::list<std::unique_ptr<GameObject>> _mGameObjects{};

	// ポーズ・ヒットストップフラグ
	// シーンによってマネージャーオブジェクトのクラスが異なるので、マネージャー側からフラグを切り替える
	static inline bool mPause{ false };
	static inline bool mHitStop{ false };

public:
	virtual ~Scene() = default;

	virtual void Initialize() = 0;
	virtual void Finalize();
	virtual void Update(double deltaTime);
	virtual void Draw() const;

	// ポーズ・ヒットストップフラグ設定
	static void SetPause(const bool& isPause) { mPause = isPause; }
	static void SetHitStop(const bool& isHitStop) { mHitStop = isHitStop; };

	// ポーズ・ヒットストップフラグ取得
	static bool IsPause() { return mPause; }
	static bool IsHitStop() { return mHitStop; }

	/*------------------------------------------------------------
		テンプレート関数
	------------------------------------------------------------*/
	// オブジェクト追加
	template <typename T>
	static T* AddGameObject() {
		auto gameObject = std::make_unique<T>();
		gameObject->Initialize();
		T* ptr = gameObject.get();
		_mGameObjects.push_back(std::move(gameObject));

		return ptr;
	}

	// オブジェクト取得（単体）
	template <typename T>
	static T* GetGameObject() {
		for (const auto& gameObject : _mGameObjects) {
			T* find = dynamic_cast<T*>(gameObject.get());
			if (find) return find;
		}
		return nullptr;
	}

	// オブジェクト取得（複数）
	template <typename T>
	static std::vector<T*> GetGameObjects() {
		std::vector<T*> objects;
		for (const auto& gameObject : _mGameObjects) {
			T* find = dynamic_cast<T*>(gameObject.get());
			if (find != nullptr) {
				objects.push_back(find);
			}
		}
		return objects;
	}
};