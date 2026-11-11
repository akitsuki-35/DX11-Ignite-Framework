/*============================================================
*	@file	 : Camera.cpp
*	@brief	 : カメラ基底クラス
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/04/26
*	@updated : 2026/11/09
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

	GameObject::Initialize();
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

void Camera::CalcProjection()
{
	// プロジェクション行列作成
	mProjectionMatrix = XMMatrixPerspectiveFovLH(1.0f,
		static_cast<float>(Screen::WIDTH) / static_cast<float>(Screen::HEIGHT), 1.0f, 1000.0f);
}

void Camera::SetMatrix() const
{
	// プロジェクション行列設定
	D3D11::BufferManager::getInstance().SetProjectionMatrix(mProjectionMatrix);

	// ビュー行列設定
	D3D11::BufferManager::getInstance().SetViewMatrix(mViewMatrix);
}

bool Camera::IsView(const Vector3& position, float size) const
{
	// フラスタムカリング

	XMMATRIX vp = mViewMatrix * mProjectionMatrix;
	XMMATRIX invVp = XMMatrixInverse(nullptr, vp);

	XMFLOAT3 vPos[4] = {
		{ -1.0, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ -1.0f, -1.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f }
	};

	XMVECTOR vPosV[4] = {
		XMLoadFloat3(&vPos[0]),
		XMLoadFloat3(&vPos[1]),
		XMLoadFloat3(&vPos[2]),
		XMLoadFloat3(&vPos[3])
	};

	XMVECTOR wPosV[4] = {
		XMVector3TransformCoord(vPosV[0], invVp),
		XMVector3TransformCoord(vPosV[1], invVp),
		XMVector3TransformCoord(vPosV[2], invVp),
		XMVector3TransformCoord(vPosV[3], invVp),
	};

	Vector3 wp[4]{};
	XMStoreFloat3((XMFLOAT3*)&wp[0], wPosV[0]);
	XMStoreFloat3((XMFLOAT3*)&wp[1], wPosV[1]);
	XMStoreFloat3((XMFLOAT3*)&wp[2], wPosV[2]);
	XMStoreFloat3((XMFLOAT3*)&wp[3], wPosV[3]);

	// 対象→カメラの方向ベクトルを取得
	Vector3 cameraPos = mTransform.GetPosition();
	Vector3 v = position - cameraPos;

	// 左面
	{
		Vector3 v1{}, v2{};
		v1 = wp[0] - cameraPos;
		v2 = wp[2] - cameraPos;

		// 法線ベクトルを求める
		Vector3 n = Vector3::Cross(v1, v2);
		n.Normalize();

		float d = Vector3::Dot(n, v);

		if (d < -size) {
			return false;
		}
	}

	// 右面
	{
		Vector3 v1{}, v2{};
		v1 = wp[3] - cameraPos;
		v2 = wp[1] - cameraPos;

		// 法線ベクトルを求める
		Vector3 n = Vector3::Cross(v1, v2);
		n.Normalize();

		float d = Vector3::Dot(n, v);

		if (d < -size) {
			return false;
		}
	}

	return true;
}