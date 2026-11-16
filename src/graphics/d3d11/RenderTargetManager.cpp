/*============================================================
*	@file	 : RenderTargetManager.cpp
*	@brief	 : レンダーターゲット管理
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/16
*	@updated : 2026/11/16
*============================================================*/
#include "RenderTargetManager.h"

D3D11::RenderTarget* D3D11::RenderTargetManager::Get(const std::string& keyName)
{
    // レンダーターゲットが存在すれば返す
    auto it = mRenderTargets.find(keyName);

    if (it != mRenderTargets.end()) {
        return it->second.get();
    }

    return nullptr;
}

bool D3D11::RenderTargetManager::Register(const RT_DESC& desc)
{
    // 登録処理
    if (desc.KeyName.empty()) {
        return false;
    }

    // 二重登録しない
    if (mRenderTargets.contains(desc.KeyName)) {
        return false;
    }

    auto rt = std::make_unique<RenderTarget>();

    if (!rt->Create(desc)) {
        return false;
    }

    mRenderTargets.emplace(desc.KeyName, std::move(rt));

    return true;
}

void D3D11::RenderTargetManager::Clear()
{
    mRenderTargets.clear();
}