/*============================================================
*	@file	 : Skeleton.cpp
*	@brief	 : ボーン構造体＆スケルトン
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/07
*	@updated : 2026/10/01
*============================================================*/
#include "Skeleton.h"
#include "Elements.h"
#include <cassert>

using namespace DirectX;

int Skeleton::AddNode(const NODE& node)
{
    // ノードをマップに登録
    int nodeIndex = static_cast<int>(mNodes.size());

    mNodes.push_back(node);
    mNodeMap.emplace(node.Name, nodeIndex);

    return nodeIndex;
}

int Skeleton::FindNode(const std::string& name) const
{
    // ノード探索
    auto it = mNodeMap.find(name);

    if (it == mNodeMap.end()) {
        return -1;
    }

    return it->second;
}

int Skeleton::AddBone(const BONE& bone)
{
    auto it = mBoneMap.find(bone.Name);

    // 既に存在すれば返す
    if (it != mBoneMap.end()) {
        return it->second;
    }

    assert(mBones.size() < Element::MAX_BONE);

    // ボーンをマップに登録
    int boneIndex = static_cast<int>(mBones.size());

    mBones.push_back(bone);
    mBoneMap.emplace(bone.Name, boneIndex);

    return boneIndex;
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
    for (size_t i = 0; i < mNodes.size(); ++i) {

        if (mNodes[i].ParentIndex == -1) {
            // グローバル行列更新
            updateGlobal(static_cast<int>(i));
        }
    }

    // スキニング行列更新
    updateSkinningMatrices();
}

void Skeleton::ToBindPose()
{
    // ノードをバインドポーズにリセット
    for (NODE& node : mNodes) {
        node.Local = node.BindLocal;
    }
}

void Skeleton::updateGlobal(int index)
{
    NODE& node = mNodes[index];

    XMMATRIX local = XMLoadFloat4x4(&node.Local);

    if (node.ParentIndex == -1) {
        XMStoreFloat4x4(&node.Global, local);
    }
    else {
        assert(node.ParentIndex < mNodes.size());

        XMMATRIX parentGlobal = XMLoadFloat4x4(&mNodes[node.ParentIndex].Global);

        XMMATRIX global = local * parentGlobal;
        XMStoreFloat4x4(&node.Global, global);
    }

    // 子を更新
    for (int i = 0; i < static_cast<int>(mNodes.size()); ++i) {
        if (mNodes[i].ParentIndex == index) {
            updateGlobal(i);
        }
    }
}

int Skeleton::GetNodeIndex(const std::string& name)
{
    // ノードインデックス取得
    auto it = mNodeMap.find(name);
    if (it != mNodeMap.end()) {
        return it->second;
    }

    return -1;
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

void Skeleton::updateSkinningMatrices()
{
    mSkinningMatrices.resize(mBones.size());

    // モデル全体の逆行列を取得
    DirectX::XMMATRIX globalInverse = XMLoadFloat4x4(&mGlobalInverse);

    for (size_t i = 0; i < mBones.size(); ++i) {
        const BONE& bone = mBones[i];

        assert(bone.NodeIndex < mNodes.size());

        const NODE& node = mNodes[bone.NodeIndex];

        // オフセット行列取得
        DirectX::XMMATRIX offset = DirectX::XMLoadFloat4x4(&bone.Offset);

        // グローバル行列取得
        DirectX::XMMATRIX global = DirectX::XMLoadFloat4x4(&node.Global);

        // スキニング行列作成
        DirectX::XMMATRIX skinning = offset * global * globalInverse;
        DirectX::XMStoreFloat4x4(&mSkinningMatrices[i], skinning);
    }
}