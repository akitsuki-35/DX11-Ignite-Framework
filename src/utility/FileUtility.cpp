/*============================================================
*	@file	 : FileUtility.cpp
*	@brief	 : ファイル関連ユーティリティ
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/09/28
*	@updated : 2026/10/05
*============================================================*/
#include "FileUtility.h"
#include <fstream>
#include <cassert>
#include <shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

std::vector<char> Utility::File::load(const char* filePath)
{
	// ファイルロード

	std::ifstream file(filePath, std::ios::binary);

	assert(file.is_open());

	// ファイルサイズ取得
	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(static_cast<size_t>(size));

	file.read(buffer.data(), size);

	return buffer;
}

std::string Utility::File::normalizePath(const char* filePath)
{
	// ファイルパス正規化

	char fullPath[MAX_PATH];

	// 絶対パス変換
	if (!GetFullPathNameA(filePath, MAX_PATH, fullPath, nullptr)) {
		return std::string(filePath);
	}

	char canonical[MAX_PATH];

	// 正規化
	if (PathCanonicalizeA(canonical, fullPath)) {
		return std::string(canonical);
	}

	// 正規化失敗時は絶対パスを返す
	return std::string(fullPath);
}

std::filesystem::path Utility::File::getDirectoryPath(const char* filePath)
{
	// ディレクトリのパス取得
	std::filesystem::path directory = filePath;
	directory = directory.parent_path();
	
	// 文字列連結によるパス組み立て用
	directory += "\\";

	return directory;
}

std::string Utility::File::getFileName(const std::string& filePath)
{
	// 生ファイル名取得
	auto slashPos = filePath.find_last_of("/\\");
	auto dotPos = filePath.find_last_of('.');

	size_t startPos = (slashPos == std::string::npos) ? 0 : slashPos + 1;

	if (dotPos != std::string::npos && dotPos > startPos) {
		return filePath.substr(startPos, dotPos - startPos);
	}

	return filePath.substr(startPos);
}

std::string Utility::File::getFileExtension(const std::string& filePath)
{
	// ファイル拡張子取得
	auto pos = filePath.find_last_of('.');

	std::string ext = filePath.substr(pos + 1);
	
	for (auto& c : ext) {
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	}

	return ext;
}