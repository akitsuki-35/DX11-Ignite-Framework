/*============================================================
*	@file	 : BoneTransform.h
*	@brief	 : ボーントランスフォーム
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/07
*	@updated : 2026/09/30
*============================================================*/
#pragma once

#include "Vector3.h"
#include "Quaternion.h"

/*============================================================
*	@class	: BoneTransform
*	@brief	: ボーントランスフォーム
*============================================================*/
class BoneTransform
{
public:
    Vector3 mPosition{};
    Quaternion mRotation{};
    Vector3 mScale{ 1.0f,1.0f,1.0f };

    // XMMATRIXに変換
    DirectX::XMMATRIX ToMatrix() const;
};