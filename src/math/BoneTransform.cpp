/*============================================================
*	@file	 : BoneTransform.cpp
*	@brief	 : ボーン用トランスフォーム
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/13
*	@updated : 2026/09/30
*============================================================*/
#include "BoneTransform.h"

DirectX::XMMATRIX BoneTransform::ToMatrix() const
{
    // XMMATRIXに変換
    DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(mScale.x, mScale.y, mScale.z);
    DirectX::XMMATRIX rotation = mRotation.ToMatrix();
    DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);

    return scale * rotation * translation;
}
