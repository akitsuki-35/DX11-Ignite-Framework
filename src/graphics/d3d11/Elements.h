/*============================================================
*	@file	 : Elements.h 
*	@brief	 : 構造体定義
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/07/14
*	@updated : 2026/10/01
*============================================================*/
#pragma once

#include <DirectXMath.h>

/*============================================================
*	@namespace	: Element 
*	@brief		: 汎用構造体定義
*============================================================*/
namespace Element {
	/*--------------------------------------------------
		頂点構造体
	----------------------------------------------------*/
	struct VERTEX3D
	{
		DirectX::XMFLOAT3 Position{};
		DirectX::XMFLOAT3 Normal{};
		DirectX::XMFLOAT4 Diffuse{};
		DirectX::XMFLOAT2 TexCoord{};
		
		// ボーン
		uint32_t BoneIndices[4]{};
		float BoneWeights[4]{};
	};

	/*--------------------------------------------------
		ボーンバッファ
	----------------------------------------------------*/
	// 最大ボーン数
	static constexpr int MAX_BONE{ 128 };
	struct BONE
	{
		DirectX::XMFLOAT4X4 Matrices[128]{};
	};

	/*--------------------------------------------------
		マテリアル構造体
	----------------------------------------------------*/
	struct MATERIAL
	{
		DirectX::XMFLOAT4 Ambient{};
		DirectX::XMFLOAT4 Diffuse{};
		DirectX::XMFLOAT4 Specular{};
		DirectX::XMFLOAT4 Emission{};
		float Shininess{};
		int TextureEnable{};
		float Dummy[2];
	};

	/*--------------------------------------------------
		ライト構造体
	----------------------------------------------------*/
	struct LIGHT
	{
		DirectX::XMFLOAT4 Position{};
		int Enable{};
		float Dummy[3]{};
		DirectX::XMFLOAT4 Direction{};
		DirectX::XMFLOAT4 Diffuse{};
		DirectX::XMFLOAT4 Ambient{};
	};
}