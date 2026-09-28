/*============================================================
*	@file	 : CSVHandler.cpp
*	@brief	 : CSVファイル読み込み・書き出し
*
* 　@author  : @akitsuki-35（https://github.com/akitsuki-35）
* 　@date	 : 2026/08/21
*	@updated : 2026/09/28
*============================================================*/
#include "CSVHandler.h"
#include "FileUtility.h"
#include <sstream>
#include <fstream>

bool CSVHandler::Load(const char* filePath, Data& data)
{
    data.clear();

    // ファイルロード
    std::vector<char> buffer = Utility::File::load(filePath);
    
    if (buffer.empty()) {
        return false;
    }

    std::string string(buffer.data(), buffer.size());
    std::stringstream stream(string);
    
    std::string line{};

    while (std::getline(stream, line)) {
        line = trim(line);

        // 空行・コメント行をスキップ
        if (line.empty() || line == "#") {
            continue;
        }

        Row row{};
        std::stringstream lineStream(line);
        std::string cell{};

        // カンマ区切りでセルに分割
        while (std::getline(lineStream, cell, ',')) {
            row.push_back(trim(cell));
        }

        if (!row.empty()) {
            data.push_back(row);
        }
    }

    return true;
}

bool CSVHandler::Export(const char* filePath, const Data& data)
{
    std::ofstream file(filePath);
    if (!file.is_open()) {
        // 保存先ディレクトリが存在しない場合はreturn
        return false;
    }

    // 2次元配列のデータをファイルに書き込み
    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << row[i];

            // 行末セル以外に区切り文字としてカンマを挿入
            if (i < row.size() - 1) {
                file << ",";
            }
        }

        // 行末で改行
        file << "\n"; 
    }

    file.close();

    return true;
}

std::string CSVHandler::GetString(const Row& row, size_t index, const std::string& defaultValue)
{
    // インデックス範囲チェック
    if (index >= row.size()) {
        // 範囲外ならデフォルト値を返す
        return defaultValue;
    }

    return row[index];
}

int CSVHandler::GetInt(const Row& row, size_t index, int defaultValue)
{
    // インデックス範囲とデータチェック
    if (index >= row.size() || row[index].empty()) {
        // 範囲外または空文字の場合はデフォルト値を返す
        return defaultValue;
    }

    const std::string& str = row[index];
    int value = 0;

    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);

    // 不正な値の場合はデフォルト値を返す
    return (ec == std::errc{}) ? value : defaultValue;
}

float CSVHandler::GetFloat(const Row& row, size_t index, float defaultValue)
{
    // インデックス範囲とデータチェック
    if (index >= row.size() || row[index].empty()) {
        // 範囲外または空文字の場合はデフォルト値を返す
        return defaultValue;
    }

    const std::string& str = row[index];
    float value = 0.0f;

    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);

    // 不正な値の場合はデフォルト値を返す
    return (ec == std::errc{}) ? value : defaultValue;
}

double CSVHandler::GetDouble(const Row& row, size_t index, double defaultValue)
{
    // インデックス範囲とデータチェック
    if (index >= row.size() || row[index].empty()) {
        // 範囲外または空文字の場合はデフォルト値を返す
        return defaultValue;
    }

    const std::string& str = row[index];
    double value = 0.0;

    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);

    // 不正な値の場合はデフォルト値を返す
    return (ec == std::errc{}) ? value : defaultValue;
}

std::string CSVHandler::trim(const std::string& str)
{
    // 削除対象とする空白文字のリスト
    const std::string whiteSpace = " \t\r\n";

    // 最初に見つかった空白以外の文字の位置
    const auto strBegin = str.find_first_not_of(whiteSpace);

    if (strBegin == std::string::npos) {
        // 中身が全て空白だった場合は空文字を返す
        return "";
    }

    // 最後に見つかった空白以外の文字の位置
    const auto strEnd = str.find_last_not_of(whiteSpace);

    // 全体長
    const auto strRange = strEnd - strBegin + 1;

    // 空白を除いた純粋なデータ部分のみを返す
    return str.substr(strBegin, strRange);
}
