/*============================================================
*	@file	 : Shadow.cpp
*	@brief	 : シャドウ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/15
*	@updated : 2026/11/09
*============================================================*/
#include "Shadow.h"
#include "SpriteRenderer.h"
#include "MeshTypes.h"

using namespace MeshType;

void Shadow::Initialize()
{
	SpriteRenderer* renderer = AddComponent<SpriteRenderer>(this);

	renderer->GetMesh().CreatePlane(Plane::Pivot::Center, Plane::Axis::XZ);
	mTransform.SetScale({ 3.0f, 3.0f, 3.0f });

	renderer->LoadTexture("assets\\textures\\shadow.png")->LoadShader("Unlit")->SetLayer(Layer::Alpha)->SetBlendState(Blend::Multiply);
	
	GameObject::Initialize();
}

void Shadow::Finalize()
{
	GameObject::Finalize();
}

void Shadow::Update(double deltaTime)
{
	GameObject::Update(deltaTime);
}

void Shadow::Draw() const
{
	GameObject::Draw();
}