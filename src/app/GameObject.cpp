/*============================================================
*	@file	 : GameObject.cpp
*	@brief	 : 3Dゲームオブジェクト基底クラス
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/09/09
*	@updated : 2026/11/25
*============================================================*/
#include "GameObject.h"
#include <algorithm>

bool GameObject::Destroy()
{
	// オブジェクト削除予約済み→オブジェクト削除
	if (mDestroy) {
		Finalize();
		return true;
	}
	else {
		return false;
	}
}

void GameObject::Initialize()
{
	std::stable_sort(_mComponents.begin(), _mComponents.end(),
		[](const std::unique_ptr<Component>& a, const std::unique_ptr<Component>& b) {
			return static_cast<int>(a->GetPriority()) > static_cast<int>(b->GetPriority());
		});
}

void GameObject::Finalize()
{
	// コンポーネントの終了処理を呼ぶ
	for (const auto& component : _mComponents) {
		component->Finalize();
	}
	_mComponents.clear();
}

void GameObject::Update(double deltaTime)
{
	// コンポーネントの更新処理を呼ぶ
	for (const auto& component : _mComponents) {
		component->Update(deltaTime);
	}
}

void GameObject::Draw() const
{
	// コンポーネントの描画処理を呼ぶ
	for (const auto& component : _mComponents) {
		component->Draw();
	}
}

GameObject& GameObject::SetPosition(const Vector3& position)
{
	mTransform.SetPosition(position);
	return *this;
}

GameObject& GameObject::SetRotation(const Vector3& rotation)
{
	mTransform.SetRotation(rotation);
	return *this;
}

GameObject& GameObject::SetScale(const Vector3& scale)
{
	mTransform.SetScale(scale);
	return *this;
}

GameObject& GameObject::SetWorldMatrix(const DirectX::XMMATRIX& world)
{
	mTransform.SetWorldMatrix(world);
	return *this;
}

GameObject& GameObject::SetTag(const std::string& tag)
{
	mTag = tag;
	return *this;
}
