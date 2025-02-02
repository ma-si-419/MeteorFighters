#include "SoundManager.h"
#include <map>
#include "DxLib.h"
#include "LoadCsv.h"

namespace
{
	constexpr int kDefaultVolume = 200;
}

SoundManager::~SoundManager()
{
}

void SoundManager::Init()
{
	LoadCsv loadCsv;

	std::vector<std::vector<std::string>> loadData = loadCsv.LoadFile("data/csv/soundData.csv");

	std::vector<std::string> sceneName;

	struct pushDataInfo
	{
		std::string name;
		std::string path;
		std::string scene;
	};

	std::vector<pushDataInfo> pathData;

	for (auto item : loadData)
	{
		//��Ɏg�p���鉹���̃f�[�^
		std::vector<std::string> utilityData;

		//Utility�̃f�[�^���擾
		if (item[static_cast<int>(FileDataSort::kFileName)] == "Utility")
		{
			std::string path = "data/sound/" + item[static_cast<int>(FileDataSort::kFileName)] + "/" + item[static_cast<int>(FileDataSort::kPath)];

			m_utilitySoundHandle[item[static_cast<int>(FileDataSort::kSoundName)]] = LoadSoundMem(path.c_str());
		}
		//Utility�ȊO�̃f�[�^���擾
		else
		{
			//�����p�X�����邽�߂̍\����
			pushDataInfo pushData;

			//�摜�̖��O��ۑ�
			pushData.name = item[static_cast<int>(FileDataSort::kSoundName)];
			//�摜�̃t�@�C������ۑ�
			pushData.path = item[static_cast<int>(FileDataSort::kFileName)];
			pushData.path += "/";
			//�摜�̃p�X��ۑ�
			pushData.path += item[static_cast<int>(FileDataSort::kPath)];
			//�摜���ǂ̃V�[���Ŏg������ۑ�
			pushData.scene = item[static_cast<int>(FileDataSort::kSceneName)];

			//�ꎞ�I�Ƀf�[�^��ۑ�
			pathData.push_back(pushData);

			//�������O�̃V�[�����o�^����Ă��Ȃ���
			bool isSame = false;

			for (auto name : sceneName)
			{
				if (name == item[static_cast<int>(FileDataSort::kSceneName)])
				{
					isSame = true;
				}
			}
			//�������O�����݂��Ă��Ȃ���Γo�^
			if (!isSame)
			{
				sceneName.push_back(item[static_cast<int>(FileDataSort::kSceneName)]);
			}
		}
	}

	//�ۑ����ꂽ�V�[���̐�������
	for (auto scene : sceneName)
	{
		std::vector<std::pair<std::string, std::string>> pushData;
		//�ۑ����ꂽ�p�X�̃f�[�^�̐�������
		for (auto item : pathData)
		{
			//�V�[���̖��O�ƃp�X�̃f�[�^�̃V�[���������ł����
			if (scene == item.scene)
			{
				std::pair<std::string, std::string> data;
				data.first = item.name;
				data.second = item.path;

				pushData.push_back(data);
			}
		}
		m_pathData[scene] = pushData;
	}
}

void SoundManager::LoadSceneSound(std::string sceneName)
{
	//�����n���h�����폜����
	if (m_sceneSoundHandle.size() > 0)
	{
		for (auto& item : m_sceneSoundHandle)
		{
			DeleteSoundMem(item.second);
		}
	}

	//�w�肳�ꂽ�V�[���̃��[�h����p�X
	std::vector<std::pair<std::string, std::string>> loadPaths = m_pathData[sceneName];

	//���������[�h����
	for (auto item : loadPaths)
	{
		std::string path = "data/sound/" + item.second;

		m_sceneSoundHandle[item.first] = LoadSoundMem(path.c_str());

		//���ʂ�ݒ肷��
		ChangeVolumeSoundMem(kDefaultVolume, m_sceneSoundHandle[item.first]);
	}
}

int SoundManager::PlayOnceSound(std::string soundName)
{
	//��Ɏg�p���鉹���̃n���h��������΂�����Đ�����
	if (m_utilitySoundHandle.find(soundName) != m_utilitySoundHandle.end())
	{
		return PlaySoundMem(m_utilitySoundHandle[soundName], DX_PLAYTYPE_BACK);
	}
	//�V�[���̉����n���h��������΂�����Đ�����
	else
	{
		return PlaySoundMem(m_sceneSoundHandle[soundName], DX_PLAYTYPE_BACK);
	}

	return -1;
}

int SoundManager::PlayLoopSound(std::string soundName)
{
	int playHandle = -1;

	//��Ɏg�p���鉹���̃n���h��������΂�����Đ�����
	if (m_utilitySoundHandle.find(soundName) != m_utilitySoundHandle.end())
	{
		playHandle = PlaySoundMem(m_utilitySoundHandle[soundName], DX_PLAYTYPE_BACK);
	}
	//�V�[���̉����n���h��������΂�����Đ�����
	else
	{
		playHandle = PlaySoundMem(m_sceneSoundHandle[soundName], DX_PLAYTYPE_LOOP);
	}

	return playHandle;
}

void SoundManager::StopLoopSound(std::string soundName)
{
	if (m_utilitySoundHandle.find(soundName) != m_utilitySoundHandle.end())
	{
		StopSoundMem(m_utilitySoundHandle[soundName]);
	}
	else
	{
		StopSoundMem(m_sceneSoundHandle[soundName]);
	}
}

bool SoundManager::IsPlayingSound(std::string soundName)
{
	if (m_utilitySoundHandle.find(soundName) != m_utilitySoundHandle.end())
	{
		return CheckSoundMem(m_utilitySoundHandle[soundName]) == 1;
	}
	else
	{
		return CheckSoundMem(m_sceneSoundHandle[soundName]) == 1;
	}
}

void SoundManager::SetSoundVolume(std::string soundName, int volume)
{
	if (m_utilitySoundHandle.find(soundName) != m_utilitySoundHandle.end())
	{
		ChangeVolumeSoundMem(volume, m_utilitySoundHandle[soundName]);
	}
	else
	{
		ChangeVolumeSoundMem(volume, m_sceneSoundHandle[soundName]);
	}
}
