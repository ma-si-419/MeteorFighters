#pragma once
#include <vector>
#include <string>
#include <map>

class LoadCsv
{
public:
	/// <summary>
	/// �t�@�C����ǂݍ���(��{�I�ɂb�r�u�ŃR���}�ŋ�؂�)
	/// </summary>
	/// <param name="path">�t�@�C���p�X</param>
	/// <returns>�ǂݍ��񂾌��ʂ̕�����</returns>
	std::vector<std::vector<std::string>> LoadFile(std::string path);

	/// <summary>
	/// ��������w�肵�������ŋ�؂�
	/// </summary>
	/// <param name="str">��؂肽��������</param>
	/// <param name="cutChar">���ŋ�؂邩</param>
	/// <returns>��؂����������z��ɂ��ĕԂ�</returns>
	std::vector<std::string> Split(const std::string& str, const char cutChar);
};

