/*============================================================
*	@file	 : Socket.cpp
*	@brief	 : ソケットコンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/08
*	@updated : 2026/11/09
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
    for (auto& [name, socket] : mSockets) {
        if (!socket.Child) {
            continue;
        }

        socket.Child->SetDestroy();
        socket.Child = nullptr;
    }

    mSockets.clear();
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
    for (auto& [name, socket] : mSockets)
    {
        if (!socket.Child) {
            continue;
        }

        // アニメーション適用済みジョイント行列
        const DirectX::XMFLOAT4X4* global = skeleton.GetGlobalMatrix(socket.Desc.ParentJoint);

        if (!global) {
            continue;
        }

        // ソケット位置を計算
        DirectX::XMMATRIX jointGlobal = DirectX::XMLoadFloat4x4(global);
        const auto& pos = socket.Desc.Position;
        const auto& rot = socket.Desc.Rotation;

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
        socket.Child->SetWorldMatrix(childWorld);
    }
}

SOCKET_DATA* Socket::FindSocket(const std::string& name)
{
    // ソケットを取得
    auto it = mSockets.find(name);

    if (it == mSockets.end()) {
        return nullptr;
    }

    return &it->second;
}

Socket* Socket::LoadCSV(const char* filePath)
{
    // CSVファイルからデータをロード
    SOCKET_DESC socket{};

    if (!CSVHandler::Load(filePath, socket)) {
        return nullptr;
    }

    if (socket.Name.empty()) {
        return nullptr;
    }

    if (mSockets.contains(socket.Name)) {
        return nullptr;
    }

    mSockets.emplace(socket.Name, std::move(socket));

    return this;
}

Socket* Socket::Attach(const std::string& name, GameObject* child)
{
    if (!child) {
        return nullptr;
    }

    // 名前に一致するソケットを取得してアタッチ
    SOCKET_DATA* socket = FindSocket(name);

    if (!socket) {
        return nullptr;
    }

    if (socket->Child) {
        return nullptr;
    }

    socket->Child = child;

    return this;
}

GameObject* Socket::Detach(const std::string& name)
{
    // 対象ソケットからオブジェクトをデタッチ
    SOCKET_DATA* socket = FindSocket(name);

    if (!socket) {
        return nullptr;
    }

    GameObject* child = socket->Child;

    socket->Child = nullptr;

    return child;
}
