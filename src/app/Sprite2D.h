/*============================================================
*	@file	 : Sprite2D.h
*	@brief	 : 2Dテクスチャオブジェクト
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/09/23
*	@updated : 2026/09/23
*============================================================*/
#pragma once

#include "GameObject.h"

/*------------------------------------------------------------
	前方宣言
------------------------------------------------------------*/
class UIRenderer;

/*============================================================
*	@class	: Sprite2D
*	@brief	: 2Dテクスチャオブジェクト
*============================================================*/
class Sprite2D final : public GameObject
{
private:
	UIRenderer* _mRenderer{ nullptr };

public:
	Sprite2D() = default;

	void Initialize() override;
	void Finalize() override;
	void Update(double deltaTime) override;
	void Draw() const override;

	Sprite2D* LoadTexture(const char* fileName);
	Sprite2D* LoadShader(std::string name);
	Sprite2D* SetParameter(DirectX::XMFLOAT4 parameter);
};