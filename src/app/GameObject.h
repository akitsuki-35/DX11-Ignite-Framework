/*============================================================
*	@file	 : GameObject.h
*	@brief	 : 3Dゲームオブジェクト基底クラス
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/05/12
*	@updated : 2026/11/25
*============================================================*/
#pragma once

#include "Vector3.h"
#include "Component.h"
#include "Transform.h"
#include <string>
#include <memory>
#include <vector>
#include <DirectXMath.h>

/*============================================================
*	@class	: GameObject
*	@brief	: ゲームオブジェクト基底クラス
*============================================================*/
class GameObject
{
protected:
	// トランスフォーム
	// オブジェクト共通の座標コントロール
	Transform mTransform{};

	// コンポーネント
	// オブジェクトからAddComponentでアタッチする
	std::vector<std::unique_ptr<Component>> _mComponents{};

	// タグ
	std::string mTag{};

	// 削除フラグ
	// 削除予約→更新処理完了後に安全にオブジェクト削除
	bool mDestroy{ false };

public:
	GameObject() = default;
	virtual ~GameObject() = default;

	virtual void Initialize();
	virtual void Finalize();
	virtual void Update(double deltaTime);
	virtual void Draw() const;

#ifndef NDEBUG
	// デバッグ用
	// コンポーネント非依存のため空実装し、オーバーライドして使う
	virtual void Debug(){}
#endif

	// 削除処理
	bool Destroy();

	// ゲッター
	const Transform& GetTransform() const { return mTransform; }
	const Vector3& GetPosition() const { return mTransform.GetPosition(); }
	const Vector3& GetRotation() const { return mTransform.GetRotation(); }
	const Vector3& GetScale() const { return mTransform.GetScale(); }
	const std::string& GetTag() const { return mTag; }
	virtual const bool IsDestroy() const { return mDestroy; }

	// セッター
	// Position・Rotation・ScaleのセットをTransformセッターのラッパーとして扱う
	GameObject& SetPosition(const Vector3& position);
	GameObject& SetRotation(const Vector3& rotation);
	GameObject& SetScale(const Vector3& scale);
	GameObject& SetWorldMatrix(const DirectX::XMMATRIX& world);
	GameObject& SetTag(const std::string& tag);
	void SetDestroy() { mDestroy = true; }
	
	/*------------------------------------------------------------
		テンプレート関数
	------------------------------------------------------------*/
	// コンポーネント追加
	template <typename T>
	T* AddComponent(GameObject* object) {
		auto component = std::make_unique<T>(object);
		T* ptr = component.get();

		_mComponents.push_back(std::move(component));
		ptr->Initialize();

		return ptr;
	}

	// コンポーネント取得
	template <typename T>
	T* GetComponent() const {
		for (const auto& component : _mComponents) {
			T* find = dynamic_cast<T*>(component.get());
			if (find) return find;
		}
		return nullptr;
	}

	// コンポーネント取得（複数）
	template <typename T>
	std::vector<T*> GetComponents() {
		std::vector<T*> components{};
		for (const auto& component : _mComponents) {
			T* find = dynamic_cast<T*>(component.get());
			if (find != nullptr) {
				components.push_back(find);
			}
		}
		return components;
	}
};