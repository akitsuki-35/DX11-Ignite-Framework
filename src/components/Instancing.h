/*============================================================
*	@file	 : Instancing.h
*	@brief	 : インスタンス描画管理コンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/05
*	@updated : 2026/11/05
*============================================================*/
#pragma once

#include "Component.h"
#include "Elements.h"
#include "StructuredBuffer.h"

#include <cstdint>
#include <vector>

/*============================================================
*	@class	: Instancing
*	@brief	: インスタンス描画情報管理
*============================================================*/
class Instancing : public Component
{
public:
    // HLSLのregister(t[n])に対応するSRVスロット
    // 例）StructuredBuffer<INSTANCE_DATA> : register(t1)
    static constexpr uint32_t BUFFER_SLOT{ 5 };

private:
    // インスタンス情報の配列
    std::vector<Element::InstanceData> mInstances{};

    // インスタンスをGPUへ転送するためのストラクチャードバッファ
    D3D11::StructuredBuffer<Element::InstanceData> mStructuredBuffer{};

    // GPU側に確保している最大インスタンス数
    uint32_t mCapacity{};

    // CPU側の内容がGPU側と異なっている場合true、次回GPUに再転送するフラグ
    bool mDirty{ false };

public:
    Instancing(GameObject* owner)
        : Component(owner) {}

    // GPUのストラクチャードバッファ生成
    bool Create(uint32_t capacity);

    // GPUへインスタンス情報を転送
    bool Upload();

    // バインド・アンバインド
    void Bind() const;
    void Unbind() const;

    // インスタンス追加
    bool Add(const Element::InstanceData& instance);

    // 全インスタンス削除
    void Clear();

    // 指定インスタンス取得
    Element::InstanceData& GetInstance(uint32_t index);
    const Element::InstanceData& GetInstance(uint32_t index) const;

    // インスタンス数取得
    uint32_t GetCount() const;

    // 最大インスタンス数取得
    uint32_t GetCapacity() const;

    // インスタンスが存在しないか
    bool IsEmpty() const;

    // GPUバッファが使用可能か
    bool IsValid() const;

    // GPU再転送フラグをオンにする
    void SetDirty();
};