/*============================================================
*	@file	 : Skeleton.cpp
*	@brief	 : ボーン構造体＆スケルトン
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/07
*	@updated : 2026/09/16
*============================================================*/
#include "Skeleton.h"
using namespace DirectX;

int Skeleton::AddBone(const Bone& bone)
{
    // 新規ボーン登録
    int index = static_cast<int>(mBones.size());
    mBones.push_back(bone);
    mBoneMap.emplace(bone.Name, index);

    return index;
}

int Skeleton::FindBone(const std::string& name) const
{
    // ボーン探索
    auto it = mBoneMap.find(name);

    if (it == mBoneMap.end()) {
        return -1;
    }

    return it->second;
}

void Skeleton::Update()
{
    for (size_t i = 0; i < mBones.size(); i++)
    {
        Bone& bone = mBones[i];

        if (bone.ParentIndex == -1) {
            // グローバル行列更新
            updateGlobal(static_cast<int>(i));
        }
    }

    // スキニング行列更新
    updateSkinningMatrices();
}

void Skeleton::updateGlobal(int index)
{
    Bone& bone = mBones[index];
    
    // ボーンのローカル行列取得
    DirectX::XMMATRIX local = DirectX::XMLoadFloat4x4(&bone.Local);

    // 親ボーンの有無を検索
    if (bone.ParentIndex == -1) {
        // 親がいない場合はLocal = Global
        DirectX::XMStoreFloat4x4(&bone.Global, local);
    }
    else {
        // 親のグローバル行列取得
        DirectX::XMMATRIX parentGlobal = DirectX::XMLoadFloat4x4(&mBones[bone.ParentIndex].Global);
        
        // 子Global = 親Global * 子Local
        DirectX::XMMATRIX global = local * parentGlobal;
        DirectX::XMStoreFloat4x4(&bone.Global, global);
    }

    // 子ボーンを更新
    for (size_t i = 0; i < mBones.size(); i++) {
        if (mBones[i].ParentIndex == index) {
            updateGlobal(static_cast<int>(i));
        }
    }
}

void Skeleton::CalculateBindPose()
{
    // バインドポーズ計算
    for (size_t i = 0; i < mBones.size(); i++) {
        if (mBones[i].ParentIndex == -1) {
            // BindGlobalを計算
            calculateBindGlobal(static_cast<int>(i));
        }
    }
}

int Skeleton::GetBoneIndex(const std::string& name)
{
    // ボーンインデックス取得
    auto it = mBoneMap.find(name);
    if (it != mBoneMap.end()) {
        return it->second;
    }

    return -1;
}

void Skeleton::calculateBindGlobal(int index)
{
    Bone& bone = mBones[index];

    // BindLocal取得
    XMMATRIX local = XMLoadFloat4x4(&bone.BindLocal);

    if (bone.ParentIndex == -1) {
        // ルートボーンのBindGlobal = BindLocal
        XMStoreFloat4x4(&bone.BindGlobal, local);
    }
    else {
        // 親ボーンのBindGlobal取得
        XMMATRIX parentBindGlobal = XMLoadFloat4x4(&mBones[bone.ParentIndex].BindGlobal);

        // 子BindGlobal = 親BindGlobal * 子BindLocal
        XMStoreFloat4x4(&bone.BindGlobal, local * parentBindGlobal);
    }

    // 子ボーンを更新
    for (size_t i = 0; i < mBones.size(); i++) {
        if (mBones[i].ParentIndex == index)
            calculateBindGlobal(static_cast<int>(i));
    }
}

void Skeleton::updateSkinningMatrices()
{
    mSkinningMatrices.resize(mBones.size());

    // モデル全体の逆行列を取得
    DirectX::XMMATRIX globalInverse = XMLoadFloat4x4(&mGlobalInverse);

    for (size_t i = 0; i < mBones.size(); ++i) {
        const Bone& bone = mBones[i];

        // アニメーション適用後のグローバル行列を取得
        DirectX::XMMATRIX global = DirectX::XMLoadFloat4x4(&bone.Global);

        // オフセット行列取得
        DirectX::XMMATRIX offset = DirectX::XMLoadFloat4x4(&bone.Offset);

        // スキニング行列作成
        DirectX::XMMATRIX skinning = offset * global * globalInverse;
        DirectX::XMStoreFloat4x4(&mSkinningMatrices[i], skinning);
    }
}