/*============================================================
*	@file	 : Socket.h
*	@brief	 : ソケットコンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/08
*	@updated : 2026/11/25
*============================================================*/
#pragma once

#include "Component.h"
#include "Vector3.h"
#include <string>

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
    std::string ParentJoint{};

    Vector3 Position{};
    Vector3 Rotation{};

    auto Members() {
        return std::tie(ParentJoint, Position, Rotation);
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
    GameObject* Child{ nullptr };
};

/*============================================================
*	@class	: Socket
*	@brief	: 子オブジェクト用ソケット
*============================================================*/
class Socket final : public Component
{
private:
    // ソケット
    SOCKET_DATA mSocket{};

    // Animatorコンポーネント
    Animator* _mAnimator{ nullptr };

public:
    Socket(GameObject* owner);

    void Finalize() override;
    
    void Update(double deltaTime) override;

    // CSVファイル読み込み
    Socket* LoadCSV(const char* filePath);

    // 子オブジェクト接続
    Socket* Attach(GameObject* child);

    // 子オブジェクトの接続を解除
    GameObject* Detach();

    // 子オブジェクト取得
    GameObject* GetChildObject();
};