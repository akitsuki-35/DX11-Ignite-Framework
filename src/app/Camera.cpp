/*============================================================
*	@file	 : Camera.cpp
*	@brief	 : カメラ基底クラス
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/04/26
*	@updated : 2026/10/09
*============================================================*/
#include "Camera.h"
#include "BufferManager.h"
#include "Config.h"

using namespace DirectX;

void Camera::Initialize()
{
	// 初期設定
	mTransform.SetPosition({ 0.0f, 5.0f, -10.0f });
	mTarget = Vector3(0.0f, 0.0f, 0.0f);
}

void Camera::Finalize()
{
	GameObject::Finalize();
}

void Camera::Update(double deltaTime)
{
	// アプリケーション側で継承することを前提とするため、基底クラス側では最低限の処理
	// 共通処理のため継承先のUpdate末尾で呼ぶ

	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	mViewMatrix = XMMatrixLookAtLH(XMLoadFloat3((XMFLOAT3*)&mTransform.GetPosition()),
		XMLoadFloat3((XMFLOAT3*)&mTarget), XMLoadFloat3(&up));

	// カメラ座標をGPUへ送信
	XMFLOAT3 position = mTransform.GetPosition().ConvertToXMFLOAT3();
	D3D11::BufferManager::getInstance().SetCamera({ position.x, position.y, position.z, 1.0 });

	GameObject::Update(deltaTime);
}

Vector3 Camera::GetForward() const
{
	// カメラ前方取得
	Vector3 forward = mTarget - mTransform.GetPosition();
	forward.Normalize();

	return forward;
}

Vector3 Camera::GetRight() const
{
	// カメラ前方からカメラ右方向を取得
	Vector3 forward = GetForward();
	Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 right = Vector3::Cross(up, forward);
	right.Normalize();

	return right;
}

void Camera::SetMatrix() const
{
	// プロジェクション行列設定
	XMMATRIX projection = XMMatrixPerspectiveFovLH(1.0f,
		static_cast<float>(Screen::WIDTH) / static_cast<float>(Screen::HEIGHT), 1.0f, 1000.0f);
	D3D11::BufferManager::getInstance().SetProjectionMatrix(projection);

	// ビュー行列設定
	D3D11::BufferManager::getInstance().SetViewMatrix(mViewMatrix);
}