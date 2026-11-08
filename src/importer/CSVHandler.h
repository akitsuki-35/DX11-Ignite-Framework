/*============================================================
*	@file	 : CSVHandler.h
*	@brief	 : CSVファイル入出力
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/21
*	@updated : 2026/11/08
*============================================================*/
#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include <string>
#include <vector>
#include <tuple>
#include <type_traits>
#include <DirectXMath.h>

/*============================================================
*	@class	: CSVHandler
*	@brief	: CSVファイル入出力
*============================================================*/
class CSVHandler final
{
private:
	CSVHandler() = delete;

public:
	// データ行
	using Row = std::vector<std::string>;
	
	// 行内のデータ
	using Data = std::vector<Row>;

	// ロード
	static bool Load(const char* filePath, Data& data);

	// 出力
	static bool Export(const char* filePath, const Data& data);

	// データからの数値取得
	static std::string GetString(const Row& row, size_t index, const std::string& defaultValue = "");
	static int GetInt(const Row& row, size_t index, int defaultValue = 0);
	static float GetFloat(const Row& row, size_t index, float defaultValue = 0.0f);
	static double GetDouble(const Row& row, size_t index, double defaultValue = 0.0);

	/*------------------------------------------------------------
		構造体ロード用テンプレート関数
	------------------------------------------------------------*/
	// 構造体を渡してロード
	template<class T>static bool Load(const char* filePath, T& desc) {
		Data data{};

		// CSVを文字列配列として読み込む
		if (!Load(filePath, data)) {
			return false;
		}

		// 一時オブジェクトに格納
		// 読み込み失敗時の部分的な変更を防止する
		T temp{};

		// メンバ数取得
		auto members = temp.Members();
		using MembersType = std::remove_reference_t<decltype(members)>;
		constexpr size_t memberCount = std::tuple_size_v<MembersType>;

		// CSVファイルの行数と構造体のメンバ数が一致するか判定
		if (data.size() != memberCount) {
			return false;
		}

		// データ読み込み
		if (!ReadDesc(data, members)) {
			return false;
		}

		// 全メンバの読み込みに成功した場合だけ反映
		desc = std::move(temp);

		return true;
	}

	// 構造体メンバ読み込み
	// 行数の文だけ再帰的に呼び出す
	template<size_t Index = 0, class Tuple>
	static bool ReadDesc(const Data& data, Tuple& members) {
		if constexpr (Index == std::tuple_size_v<Tuple>) {
			return true;
		}
		else {
			if (!Read(data[Index], std::get<Index>(members))) {
				return false;
			}

			return ReadDesc<Index + 1>(data, members);
		}
	}

	// 構造体メンバの型ごとの実データ読み込み
	// 渡された型で判別する
	template<class T>
	static bool Read(const Row& row, T& value) {
		// std::string
		if constexpr (std::is_same_v<T, std::string>) {
			value = GetString(row, 1);
		}

		// int
		else if constexpr (std::is_same_v<T, int>) {
			value = GetInt(row, 1);
		}

		// float
		else if constexpr (std::is_same_v<T, float>) {
			value = GetFloat(row, 1);
		}

		// double
		else if constexpr (std::is_same_v<T, double>) {
			value = GetDouble(row, 1);
		}

		// Vector2
		else if constexpr (std::is_same_v<T, Vector2>) {
			if (row.size() < 3) return false;

			value = { GetFloat(row, 1), GetFloat(row, 2) };
		}

		// Vector3
		else if constexpr (std::is_same_v<T, Vector3>) {
			if (row.size() < 4) return false;

			value = { GetFloat(row, 1), GetFloat(row, 2), GetFloat(row, 3) };
		}

		// XMFLOAT2
		else if constexpr (std::is_same_v<T, DirectX::XMFLOAT2>) {
			if (row.size() < 3) return false;

			value = { GetFloat(row, 1), GetFloat(row, 2) };
		}

		// XMFLOAT3
		else if constexpr (std::is_same_v<T, DirectX::XMFLOAT3>) {
			if (row.size() < 4) return false;

			value = { GetFloat(row, 1), GetFloat(row, 2), GetFloat(row, 3) };
		}

		// XMFLOAT4
		else if constexpr (std::is_same_v<T, DirectX::XMFLOAT4>) {
			if (row.size() < 5) return false;

			value = { GetFloat(row, 1), GetFloat(row, 2), GetFloat(row, 3), GetFloat(row, 4)
			};
		}

		// 一致する型が存在しない
		else {
			return false;
		}

		return true;
	}

private:
	// 空白のトリム
	static std::string trim(const std::string& str);
};