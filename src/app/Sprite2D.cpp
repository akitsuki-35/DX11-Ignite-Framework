/*============================================================
*	@file	 : Sprite2D.cpp
*	@brief	 : 2Dテクスチャオブジェクト
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/09/23
*	@updated : 2026/09/23
*============================================================*/
#include "Sprite2D.h"
#include "UIRenderer.h"
#include "Config.h"

void Sprite2D::Initialize()
{
	_mRenderer = AddComponent<UIRenderer>(this);

	// 回転に対応するため中央をピボットにする
	_mRenderer->GetCanvas().CreateCanvas(UIStyle::Pivot::Center);

	mTransform.SetPosition({ Screen::CENTER_X, Screen::CENTER_Y, 0.0f });
	mTransform.SetScale({ 1.0f, 1.0f, 0.0f });

	_mRenderer->LoadTexture("assets\\textures\\white.png")->LoadShader("UI");
}

void Sprite2D::Finalize()
{
	GameObject::Finalize();
}

void Sprite2D::Update(double deltaTime)
{
	GameObject::Update(deltaTime);
}

void Sprite2D::Draw() const
{
	GameObject::Draw();
}

Sprite2D* Sprite2D::LoadTexture(const char* fileName)
{
	_mRenderer->LoadTexture(fileName);
	return this;
}

Sprite2D* Sprite2D::LoadShader(std::string name)
{
	_mRenderer->LoadShader(name);
	return this;
}

Sprite2D* Sprite2D::SetParameter(DirectX::XMFLOAT4 parameter)
{
	_mRenderer->SetParameter(parameter);
	return this;
}
