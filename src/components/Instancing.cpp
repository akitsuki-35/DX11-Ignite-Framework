/*============================================================
*	@file	 : Instancing.cpp
*	@brief	 : インスタンス描画管理コンポーネント
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/05
*	@updated : 2026/11/05
*============================================================*/
#include "Instancing.h"

bool Instancing::Create(uint32_t capacity)
{
    // インスタンスを1個も保持できない設定は無効
    if (capacity == 0) {
        return false;
    }

    // CPU側データを初期状態に戻す
    mInstances.clear();

    // 最大インスタンス数までCPU側領域を予約
    mInstances.reserve(capacity);

    mCapacity = capacity;
    mDirty = false;

    // GPU側にも同じ最大要素数のストラクチャードバッファを確保
    return mStructuredBuffer.Create(capacity);
}

bool Instancing::Upload()
{
    // GPUバッファが生成されていない場合は転送しない
    if (!mStructuredBuffer.IsValid()) {
        return false;
    }

    // 最大数を超えるデータは転送しない
    if (mInstances.size() > mCapacity) {
        return false;
    }

    // CPU側に変更がなければGPUに転送しない
    if (!mDirty) {
        return true;
    }

    // インスタンスデータをストラクチャードバッファに転送
    const bool result = mStructuredBuffer.Update(mInstances);

    // 正常に転送できた場合はCPUとGPUが同期済みになる
    if (result) {
        mDirty = false;
    }

    return result;
}

void Instancing::Bind() const
{
    // HLSL側のregister(t1)へSRVを設定
    mStructuredBuffer.BindVS(BUFFER_SLOT);
}

void Instancing::Unbind() const
{
    // 前回のSRVが残らないよう解除
    mStructuredBuffer.UnbindVS(BUFFER_SLOT);
}

bool Instancing::Add(const Element::InstanceData& instance)
{
    // 最大数を超えないようにする
    if (mInstances.size() >= mCapacity) {
        return false;
    }

    mInstances.push_back(instance);

    // 再転送オン
    mDirty = true;

    return true;
}

void Instancing::Clear()
{
    mInstances.clear();

    // GPU側の有効要素数も更新する
    mDirty = true;
}

Element::InstanceData& Instancing::GetInstance(uint32_t index)
{
    assert(index < mInstances.size());

    // 非const参照を返すため、呼び出し側で内容が変更される可能性がある
    mDirty = true;

    return mInstances[index];
}

const Element::InstanceData& Instancing::GetInstance(uint32_t index) const
{
    assert(index < mInstances.size());

    return mInstances[index];
}

uint32_t Instancing::GetCount() const
{
    return static_cast<uint32_t>(mInstances.size());
}

uint32_t Instancing::GetCapacity() const
{
    return mCapacity;
}

bool Instancing::IsEmpty() const
{
    return mInstances.empty();
}

bool Instancing::IsValid() const
{
    return mStructuredBuffer.IsValid();
}

void Instancing::SetDirty()
{
    mDirty = true;
}