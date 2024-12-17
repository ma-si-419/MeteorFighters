#include "SoundManager.h"
#include <map>
#include "DxLib.h"
#include "LoadCsv.h"

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
		pushDataInfo pushData;

		//�摜�̖��O��ۑ�
		pushData.name = item[static_cast<int>(FileDataSort::kSoundName)];
		//�摜�̃p�X��ۑ�
		pushData.path = item[static_cast<int>(FileDataSort::kPath)];
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
	//�摜�n���h�����폜����
	if (m_sceneSoundHandle.size() > 0)
	{
		for (auto& item : m_sceneSoundHandle)
		{
			DeleteSoundMem(item.second);
		}
	}

	//�w�肳�ꂽ�V�[���̃��[�h����p�X
	std::vector<std::pair<std::string, std::string>> loadPaths = m_pathData[sceneName];

	//�摜�����[�h����
	for (auto item : loadPaths)
	{
		std::string path = "data/sound/" + item.second;

		m_sceneSoundHandle[item.first] = LoadSoundMem(path.c_str());
	}
}

int SoundManager::OncePlaySound(std::string soundName)
{
	return PlaySoundMem(m_sceneSoundHandle[soundName],DX_PLAYTYPE_BACK);
}

int SoundManager::LoopPlaySound(std::string soundName)
{
	int playHandle = PlaySoundMem(m_sceneSoundHandle[soundName], DX_PLAYTYPE_LOOP);
	
	m_loopPlayHandles.push_back(playHandle);

	return playHandle;
}

void SoundManager::StopLoopSound(int playHandle)
{
	StopSoundMem(playHandle);

	//�z�񂩂�폜����
	auto iterator = std::remove_if(m_loopPlayHandles.begin(), m_loopPlayHandles.end(),
		[](auto item)
		{
			if (item == playHandle)
			{
				StopSoundMem(playHandle);

				return true;
			}

			return false;
		});

	m_loopPlayHandles.erase(iterator,m_loopPlayHandles.end());
}
