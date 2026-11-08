/*============================================================
*	@file	 : Socket.cpp
*	@brief	 : ソケットコンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/08
*	@updated : 2026/11/08
*============================================================*/
#include "Socket.h"
#include "CSVHandler.h"
#include "Animator.h"
#include "GameObject.h"
#include "Transform.h"

Socket::Socket(GameObject* owner)
{
    _mOwner = owner;
    _mAnimator = _mOwner->GetComponent<Animator>();
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
    _mAnimator = nullptr;
}

void Socket::Update(double deltaTime)
{
    if (!_mAnimator) {
        return;
    }

    // スケルトン取得
    Skeleton& skeleton = _mAnimator->GetSkeleton();

    // ソケット更新
    for (auto& [name, socket] : mSockets)
    {
        if (!socket.Child) {
            continue;
        }

        int nodeIndex = skeleton.GetNodeIndex(socket.Desc.ParentJoint);

        if (nodeIndex < 0) {
            continue;
        }

        Skeleton::NODE& node = skeleton.GetNode(nodeIndex);

        // ソケット位置を計算
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

    socket->Child->SetPosition(_mOwner->GetPosition());

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
