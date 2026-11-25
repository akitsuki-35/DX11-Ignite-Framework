/*============================================================
*	@file	 : Socket.cpp
*	@brief	 : ソケットコンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/08
*	@updated : 2026/11/25
*============================================================*/
#include "Socket.h"
#include "CSVHandler.h"
#include "Animator.h"
#include "GameObject.h"
#include "Transform.h"

Socket::Socket(GameObject* owner)
    :Component(owner)
{
    // アニメーション後の行列を使うため、Animatorより後に処理する
    mPriority = Priority::Medium;
}

void Socket::Finalize()
{
    // 子オブジェクトの終了処理
    if (mSocket.Child) {
        mSocket.Child->SetDestroy();
        mSocket.Child = nullptr;
    }
}

void Socket::Update(double /*deltaTime*/)
{
    // Animatorコンポーネントがnullの場合は取得する
    if (!_mAnimator) {
        Animator* animator = _mOwner->GetComponent<Animator>();

        if (!animator) {
            return;
        }

        _mAnimator = animator;
    }

    // スケルトン取得
    Skeleton& skeleton = _mAnimator->GetSkeleton();

    // 親のワールド行列
    DirectX::XMMATRIX ownerWorld = _mOwner->GetTransform().GetWorldMatrix();

    // ソケット更新
    if (mSocket.Child) {
        // アニメーション適用済みジョイント行列
        const DirectX::XMFLOAT4X4* global = skeleton.GetGlobalMatrix(mSocket.Desc.ParentJoint);

        if (global) {
            // ソケット位置を計算
            DirectX::XMMATRIX jointGlobal = DirectX::XMLoadFloat4x4(global);
            const auto& pos = mSocket.Desc.Position;
            const auto& rot = mSocket.Desc.Rotation;

            // Mayaの回転は度数方式なのでラジアンに変換する
            DirectX::XMMATRIX socketRotation = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(rot.x),
                DirectX::XMConvertToRadians(rot.y), DirectX::XMConvertToRadians(rot.z));

            DirectX::XMMATRIX socketTranslation = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

            DirectX::XMMATRIX socketMatrix = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) *
                DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

            // 親から見たソケットのローカル行列
            DirectX::XMMATRIX socketLocal = socketRotation * socketTranslation;

            // 子オブジェクトのワールド行列をセット
            DirectX::XMMATRIX childWorld = socketLocal * jointGlobal * ownerWorld;
            mSocket.Child->SetWorldMatrix(childWorld);
        }
    }
}

Socket* Socket::LoadCSV(const char* filePath)
{
    // CSVファイルからデータをロード
    SOCKET_DESC socket{};

    if (!CSVHandler::Load(filePath, socket)) {
        return nullptr;
    }

    mSocket.Desc = socket;

    return this;
}

Socket* Socket::Attach(GameObject* child)
{
    if (!child) {
        return nullptr;
    }

    // 名前に一致するソケットを取得してアタッチ
    if (mSocket.Child) {
        return nullptr;
    }

    mSocket.Child = child;

    return this;
}

GameObject* Socket::Detach()
{
    // 対象ソケットからオブジェクトをデタッチ
    GameObject* child = mSocket.Child;

    mSocket.Child = nullptr;

    return child;
}

GameObject* Socket::GetChildObject()
{
    // 指定ソケットの子オブジェクトを取得
    return mSocket.Child;
}
