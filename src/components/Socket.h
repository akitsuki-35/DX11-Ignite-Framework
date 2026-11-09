/*============================================================
*	@file	 : Socket.h
*	@brief	 : ソケットコンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/08
*	@updated : 2026/11/09
*============================================================*/
#pragma once

#include "Component.h"
#include "Vector3.h"
#include <string>
#include <unordered_map>
#include <DirectXMath.h>

/*------------------------------------------------------------
	前方宣言
------------------------------------------------------------*/
class GameObject;
class Animator;

/*------------------------------------------------------------
	ソケットデータ構造体
------------------------------------------------------------*/
struct SOCKET_DESC
{
    std::string Name{};
    std::string ParentJoint{};

    Vector3 Position{};
    Vector3 Rotation{};

    auto Members() {
        return std::tie(Name, ParentJoint, Position, Rotation);
    }
};

/*------------------------------------------------------------
    ソケット構造体
------------------------------------------------------------*/
struct SOCKET_DATA
{
    // 定義データ
    SOCKET_DESC Desc{};

    // 子オブジェクト
    GameObject* Child = nullptr;
};

/*============================================================
*	@class	: Socket
*	@brief	: 子オブジェクト用ソケット
*============================================================*/
class Socket final : public Component
{
private:
    // ソケット配列
    std::unordered_map<std::string, SOCKET_DATA> mSockets{};

    // Animatorコンポーネント
    Animator* _mAnimator{ nullptr };

public:
    Socket(GameObject* owner);

    void Finalize() override;
    
    void Update(double deltaTime) override;

    // ソケット探索
    SOCKET_DATA* FindSocket(const std::string& name);

    // CSVファイル読み込み
    Socket* LoadCSV(const char* filePath);

    // 子オブジェクト接続
    Socket* Attach(const std::string& name, GameObject* child);

    // 子オブジェクトの接続を解除
    GameObject* Detach(const std::string& name);
};