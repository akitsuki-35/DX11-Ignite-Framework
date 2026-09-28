/*============================================================
*	@file	 : StringUtility.cpp
*	@brief	 : 文字列関連ユーティリティ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/09/28
*	@updated : 2026/09/28
*============================================================*/
#include "StringUtility.h"

#include <cassert>
#include <shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

std::wstring Utility::String::toWideString(const std::string& string)
{
	// std::string→std::wstringに変換

	if (string.empty())
	{
		return{};
	}

	// 終端文字を含む文字列を取得
	const int size = MultiByteToWideChar(CP_ACP, 0, string.c_str(), -1, nullptr, 0);

	assert(size > 0);

	std::wstring wide(size - 1, L'\0');

	MultiByteToWideChar(CP_ACP, 0, string.c_str(), -1, wide.data(), size);

	return wide;
}