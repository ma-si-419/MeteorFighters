#pragma once
#include <vector>
#include <string>
#include <map>

class LoadCsv
{
public:
	/// <summary>
	/// ファイルを読み込む(基本的にＣＳＶでコンマで区切る)
	/// </summary>
	/// <param name="path">ファイルパス</param>
	/// <returns>読み込んだ結果の文字列</returns>
	std::vector<std::vector<std::string>> LoadFile(std::string path);

	/// <summary>
	/// 文字列を指定した文字で区切る
	/// </summary>
	/// <param name="str">区切りたい文字列</param>
	/// <param name="cutChar">何で区切るか</param>
	/// <returns>区切った文字列を配列にして返す</returns>
	std::vector<std::string> Split(const std::string& str, const char cutChar);
};

