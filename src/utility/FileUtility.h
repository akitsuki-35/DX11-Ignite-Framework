/*============================================================
*	@file	 : FileUtility.h
*	@brief	 : ファイル関連ユーティリティ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/09/28
*	@updated : 2026/10/05
*============================================================*/
#pragma once

#include <string>
#include <vector>
#include <filesystem>

/*============================================================
*	@namespace	: Utility::File
*	@brief		: ファイル関連ユーティリティ関数群
*============================================================*/
namespace Utility {
	namespace File {
		// ファイルロード
		std::vector<char> load(const char* filePath);

		// ファイルパス正規化
		std::string normalizePath(const char* filePath);

		// ディレクトリのパス取得
		std::filesystem::path getDirectoryPath(const char* filePath);

		// ディレクトリ名パス無しのファイル名取得
		std::string getFileName(const std::string& filePath);

		// ファイル拡張子取得
		std::string getFileExtension(const std::string& filePath);
	}
}