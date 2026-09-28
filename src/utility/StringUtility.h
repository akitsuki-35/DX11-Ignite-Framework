/*============================================================
*	@file	 : StringUtility.h
*	@brief	 : 文字列関連ユーティリティ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/09/28
*	@updated : 2026/09/28
*============================================================*/
#pragma once

#include <string>

/*============================================================
*	@namespace	: Utility::String
*	@brief		: 文字列関連ユーティリティ関数群
*============================================================*/
namespace Utility {
	namespace String {
		// std::string→std::wstringに変換
		std::wstring toWideString(const std::string& string);
	}
}