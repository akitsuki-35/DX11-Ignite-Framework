/*============================================================
*	@file	 : Skeleton.h
*	@brief	 : スケルトン
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/07
*	@updated : 2026/10/01
*============================================================*/
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <DirectXMath.h>

/*============================================================
*	@class	: Skeleton
*	@brief	: スケルトン
*============================================================*/
class Skeleton final
{
public:
    // ノード構造体
    struct NODE
    {
        // ノード名
        std::string Name{};

        // 親ノードインデックス
        int ParentIndex{ -1 };

        // バインドポーズ
        DirectX::XMFLOAT4X4 BindLocal = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        DirectX::XMFLOAT4X4 Local = BindLocal;
        DirectX::XMFLOAT4X4 Global = BindLocal;
    };

    // ボーン構造体
    struct BONE
    {
        // ボーン名
        std::string Name{};

        // 対応ノードインデックス
        int NodeIndex{ -1 };

        // 親ボーンインデックス
        int ParentIndex{ -1 };

        // オフセット行列
        DirectX::XMFLOAT4X4 Offset = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    };

private:
    // ノードマップ・配列
    std::unordered_map<std::string, int> mNodeMap{};
    std::vector<NODE> mNodes{};

    // ボーンマップ・配列
    std::unordered_map<std::string, int> mBoneMap{};
    std::vector<BONE> mBones{};

    // スキニング行列
    std::vector<DirectX::XMFLOAT4X4>mSkinningMatrices{};

    // グローバル逆行列
    DirectX::XMFLOAT4X4 mGlobalInverse = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

public:
    // ノード登録・取得
    int AddNode(const NODE& node);
    int FindNode(const std::string& name) const;

    // ボーン登録・取得
    int AddBone(const BONE& bone);
    int FindBone(const std::string& name) const;

    // 更新
    void Update();

    // ノードをバインドポーズにリセット
    void ToBindPose();

    // グローバル逆行列をセット
    void SetGlobalInverse(const DirectX::XMFLOAT4X4& matrix){ mGlobalInverse = matrix; }

    // ノード関連ゲッター
    int GetNodeIndex(const std::string& name);
    NODE& GetNode(size_t index) { return mNodes[index]; }
    size_t GetNodeCount() const { return mNodes.size(); }

    // ボーン関連ゲッター
    int GetBoneIndex(const std::string& name);
    BONE& GetBone(size_t index) { return mBones[index]; }
    size_t GetBoneCount() const { return mBones.size(); }

    // スキニング行列取得
    const std::vector<DirectX::XMFLOAT4X4>& GetSkinningMatrices() const { return mSkinningMatrices; }

private:
    // グローバル行列更新
    void updateGlobal(int index);

    // スキニング行列更新
    void updateSkinningMatrices();
};