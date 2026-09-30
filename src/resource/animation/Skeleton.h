/*============================================================
*	@file	 : Skeleton.h
*	@brief	 : スケルトン
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/07
*	@updated : 2026/09/16
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
    // ボーン構造体
    struct BONE
    {
        // ボーン名
        std::string Name{};

        // 親ボーンインデックス
        int ParentIndex{ -1 };

        // オフセット行列
        DirectX::XMFLOAT4X4 Offset{};

        // ローカル行列
        DirectX::XMFLOAT4X4 Local{};

        // グローバル行列
        DirectX::XMFLOAT4X4 Global{};

        // バインドポーズ
        DirectX::XMFLOAT4X4 BindLocal{};
        DirectX::XMFLOAT4X4 BindGlobal{};
    };

private:
    // ボーン取得用マップ
    std::unordered_map<std::string, int> mBoneMap{};

    // ボーン配列
    std::vector<BONE> mBones{};

    // スキニング行列
    std::vector<DirectX::XMFLOAT4X4>mSkinningMatrices{};

    // グローバル逆行列
    DirectX::XMFLOAT4X4 mGlobalInverse{};

public:
    // ボーン登録
    int AddBone(const BONE& bone);

    // ボーン取得
    int FindBone(const std::string& name) const;

    // 更新
    void Update();

    // バインドポーズ計算
    void CalculateBindPose();

    // グローバル逆行列をセット
    void SetGlobalInverse(const DirectX::XMFLOAT4X4& matrix){ mGlobalInverse = matrix; }

    // ゲッター
    int GetBoneIndex(const std::string& name);
    BONE& GetBone(size_t index) { return mBones[index]; }
    size_t GetBoneCount() const { return mBones.size(); }
    const std::vector<DirectX::XMFLOAT4X4>& GetSkinningMatrices() const { return mSkinningMatrices; }

private:
    // グローバル行列算出
    void calculateBindGlobal(int index);
    
    // グローバル行列更新
    void updateGlobal(int index);

    // スキニング行列更新
    void updateSkinningMatrices();
};