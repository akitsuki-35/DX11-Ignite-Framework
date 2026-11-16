/*============================================================
*	@file	 : RenderTargetManager.h
*	@brief	 : レンダーターゲット管理
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/11/16
*	@updated : 2026/11/16
*============================================================*/
#pragma once

#include "RenderTarget.h"
#include <string>
#include <memory>
#include <unordered_map>

/*============================================================
*	@class	: D3D11::RenderTargetManager
*	@brief	: レンダーターゲット管理
*============================================================*/
namespace D3D11 {
	class RenderTargetManager final
	{
	/*--------------------------------------------------
		Singleton用
	----------------------------------------------------*/
	public:
		static RenderTargetManager& getInstance() {
			static RenderTargetManager instance;
			return instance;
		}

	private:
		RenderTargetManager() = default;
		RenderTargetManager(const RenderTargetManager&) = delete;

		RenderTargetManager& operator=(const RenderTargetManager&) = delete;
		RenderTargetManager(RenderTargetManager&&) = delete;

		RenderTargetManager& operator=(RenderTargetManager&&) = delete;
		~RenderTargetManager() {};

	/*--------------------------------------------------
		メンバ変数・メンバ関数
	----------------------------------------------------*/
	private:
		// レンダーターゲット配列
		std::unordered_map<std::string, std::unique_ptr<RenderTarget>> mRenderTargets{};

	public:
		// 取得
		RenderTarget* Get(const std::string& keyName);

		// 登録
		bool Register(const RT_DESC& desc);

		// クリア
		void Clear();
	};
}