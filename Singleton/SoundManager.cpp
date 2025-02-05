#include "SoundManager.h"
#include <map>
#include "DxLib.h"
#include "LoadCsv.h"

namespace
{
	//�f�t�H���g�̃T�E���h�̔{��
	const float kDefaultVolume = 0.5f;
}

SoundManager::~SoundManager()
{
}

void SoundManager::Init()
{

	//���ʂ̐ݒ�
	m_seVolume = kDefaultVolume;
	m_bgmVolume = kDefaultVolume;
	m_voiceVolume = kDefaultVolume;
	m_masterVolume = kDefaultVolume;


	LoadCsv loadCsv;

	std::vector<std::vector<std::string>> loadData = loadCsv.LoadFile("data/csv/soundData.csv");

	std::vector<std::string> sceneName;

	struct CsvDataInfo
	{
		std::string name;
		std::string path;
		std::string scene;
		float maxVolume;
		SoundKind kind;
	};

	std::vector<CsvDataInfo> pathData;

	for (auto item : loadData)
	{
		//��Ɏg�p���鉹���̃f�[�^
		std::vector<std::string> utilityData;

		//Utility�̃f�[�^���擾
		if (item[static_cast<int>(FileDataSort::kFileName)] == "Utility")
		{
			std::string path = "data/sound/" + item[static_cast<int>(FileDataSort::kFileName)] + "/" + item[static_cast<int>(FileDataSort::kPath)];

			auto handle = LoadSoundMem(path.c_str());

			m_utilitySoundData[item[static_cast<int>(FileDataSort::kSoundName)]].handle = handle;
			m_utilitySoundData[item[static_cast<int>(FileDataSort::kSoundName)]].maxVolume = stof(item[static_cast<int>(FileDataSort::kMaxVolume)]);

			auto itemKind = item[static_cast<int>(FileDataSort::kSoundKind)];

			auto maxVolume = std::stof(item[static_cast<int>(FileDataSort::kMaxVolume)]);

			//���ʂ�ݒ肷��
			//SE�̏ꍇ
			if (itemKind == "SE")
			{
				ChangeVolumeSoundMem(maxVolume * m_seVolume * m_masterVolume, handle);
				m_utilitySoundData[item[static_cast<int>(FileDataSort::kSoundName)]].kind = SoundKind::kSe;
			}
			//BGM�̏ꍇ
			else if (itemKind == "BGM")
			{
				ChangeVolumeSoundMem(maxVolume * m_bgmVolume * m_masterVolume, handle);
				m_utilitySoundData[item[static_cast<int>(FileDataSort::kSoundName)]].kind = SoundKind::kBgm;
			}
			//�{�C�X�̏ꍇ
			else if (itemKind == "VOICE")
			{
				ChangeVolumeSoundMem(maxVolume * m_voiceVolume * m_masterVolume, handle);
				m_utilitySoundData[item[static_cast<int>(FileDataSort::kSoundName)]].kind = SoundKind::kVoice;
			}
		}
		//Utility�ȊO�̃f�[�^���擾
		else
		{
			//�����p�X�����邽�߂̍\����
			CsvDataInfo pushData;

			//�摜�̖��O��ۑ�
			pushData.name = item[static_cast<int>(FileDataSort::kSoundName)];
			//�摜�̃t�@�C������ۑ�
			pushData.path = item[static_cast<int>(FileDataSort::kFileName)];
			pushData.path += "/";
			//�摜�̃p�X��ۑ�
			pushData.path += item[static_cast<int>(FileDataSort::kPath)];
			//�摜���ǂ̃V�[���Ŏg������ۑ�
			pushData.scene = item[static_cast<int>(FileDataSort::kSceneName)];
			//���ʂ�ۑ�
			pushData.maxVolume = std::stof(item[static_cast<int>(FileDataSort::kMaxVolume)]);

			//�����̎�ނ�ۑ�
			auto kind = item[static_cast<int>(FileDataSort::kSoundKind)];
			if (kind == "BGM")
			{
				pushData.kind = SoundKind::kBgm;
			}
			else if (kind == "SE")
			{
				pushData.kind = SoundKind::kSe;
			}
			else if (kind == "VOICE")
			{
				pushData.kind = SoundKind::kVoice;
			}

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
		std::vector<SoundData> pushData;
		//�ۑ����ꂽ�p�X�̃f�[�^�̐�������
		for (auto item : pathData)
		{
			//�V�[���̖��O�ƃp�X�̃f�[�^�̃V�[���������ł����
			if (scene == item.scene)
			{
				SoundData data;
				data.name = item.name;
				data.path = item.path;
				data.kind = item.kind;
				data.maxVolume = item.maxVolume;

				pushData.push_back(data);
			}
		}

		m_soundData[scene] = pushData;
	}
}

void SoundManager::LoadSceneSound(std::string sceneName)
{
	//�����n���h�����폜����
	if (m_sceneSoundData.size() > 0)
	{
		for (auto& item : m_sceneSoundData)
		{
			DeleteSoundMem(item.second.handle);
		}
	}

	//�w�肳�ꂽ�V�[���̃��[�h����p�X
	std::vector<SoundData> loadSoundData = m_soundData[sceneName];

	//���������[�h����
	for (auto item : loadSoundData)
	{
		std::string path = "data/sound/" + item.path;

		m_sceneSoundData[item.name].handle = LoadSoundMem(path.c_str());

		//���ʂ�ݒ肷��

		float volume = 0;

		//SE�̏ꍇ
		if (item.kind == SoundKind::kSe)
		{
			volume = item.maxVolume * m_seVolume * m_masterVolume;

			m_sceneSoundData[item.name].kind = SoundKind::kSe;
			m_sceneSoundData[item.name].maxVolume = item.maxVolume;

			ChangeVolumeSoundMem(volume, m_sceneSoundData[item.name].handle);
		}
		//BGM�̏ꍇ
		else if (item.kind == SoundKind::kBgm)
		{
			volume = item.maxVolume * m_bgmVolume * m_masterVolume;

			m_sceneSoundData[item.name].kind = SoundKind::kBgm;
			m_sceneSoundData[item.name].maxVolume = item.maxVolume;

			ChangeVolumeSoundMem(volume, m_sceneSoundData[item.name].handle);
		}
		//�{�C�X�̏ꍇ
		else if (item.kind == SoundKind::kVoice)
		{
			volume = item.maxVolume * m_voiceVolume * m_masterVolume;

			m_sceneSoundData[item.name].kind = SoundKind::kVoice;
			m_sceneSoundData[item.name].maxVolume = item.maxVolume;

			ChangeVolumeSoundMem(volume, m_sceneSoundData[item.name].handle);
		}
	}
}

int SoundManager::PlayOnceSound(std::string soundName)
{
	//��Ɏg�p���鉹���̃n���h��������΂�����Đ�����
	if (m_utilitySoundData.find(soundName) != m_utilitySoundData.end())
	{
		return PlaySoundMem(m_utilitySoundData[soundName].handle, DX_PLAYTYPE_BACK);
	}
	//�V�[���̉����n���h��������΂�����Đ�����
	else
	{
		return PlaySoundMem(m_sceneSoundData[soundName].handle, DX_PLAYTYPE_BACK);
	}

	return -1;
}

int SoundManager::PlayLoopSound(std::string soundName)
{
	int playHandle = -1;

	//��Ɏg�p���鉹���̃n���h��������΂�����Đ�����
	if (m_utilitySoundData.find(soundName) != m_utilitySoundData.end())
	{
		playHandle = PlaySoundMem(m_utilitySoundData[soundName].handle, DX_PLAYTYPE_BACK);
	}
	//�V�[���̉����n���h��������΂�����Đ�����
	else
	{
		playHandle = PlaySoundMem(m_sceneSoundData[soundName].handle, DX_PLAYTYPE_LOOP);
	}

	return playHandle;
}

void SoundManager::StopSound(std::string soundName)
{
	if (m_utilitySoundData.find(soundName) != m_utilitySoundData.end())
	{
		StopSoundMem(m_utilitySoundData[soundName].handle);
	}
	else
	{
		StopSoundMem(m_sceneSoundData[soundName].handle);
	}
}

void SoundManager::StopLoopSound(std::string soundName)
{
	if (m_utilitySoundData.find(soundName) != m_utilitySoundData.end())
	{
		StopSoundMem(m_utilitySoundData[soundName].handle);
	}
	else
	{
		StopSoundMem(m_sceneSoundData[soundName].handle);
	}
}

bool SoundManager::IsPlayingSound(std::string soundName)
{
	if (m_utilitySoundData.find(soundName) != m_utilitySoundData.end())
	{
		return CheckSoundMem(m_utilitySoundData[soundName].handle) == 1;
	}
	else
	{
		return CheckSoundMem(m_sceneSoundData[soundName].handle) == 1;
	}
}

void SoundManager::SetVolume(OptionSoundKind kind, float soundVolume)
{
	//SE�̉��ʂ�ݒ�
	if (kind == OptionSoundKind::kSe)
	{
		m_seVolume = soundVolume;

		//�S�Ẳ��ʂ�ݒ�
		for (auto& item : m_utilitySoundData)
		{
			//SE�̉��ʂ�ݒ�
			if (item.second.kind == SoundKind::kSe)
			{
				ChangeVolumeSoundMem(soundVolume * m_masterVolume * item.second.maxVolume, item.second.handle);
			}
		}
		for (auto& item : m_sceneSoundData)
		{
			//SE�̉��ʂ�ݒ�
			if (item.second.kind == SoundKind::kSe)
			{
				ChangeVolumeSoundMem(soundVolume * m_masterVolume * item.second.maxVolume, item.second.handle);
			}
		}

	}
	//BGM�̉��ʂ�ݒ�
	else if (kind == OptionSoundKind::kBgm)
	{
		m_bgmVolume = soundVolume;
		//�S�Ẳ��ʂ�ݒ�
		for (auto& item : m_utilitySoundData)
		{
			//BGM�̉��ʂ�ݒ�
			if (item.second.kind == SoundKind::kBgm)
			{
				ChangeVolumeSoundMem(soundVolume * m_masterVolume * item.second.maxVolume, item.second.handle);
			}
		}
		for (auto& item : m_sceneSoundData)
		{
			//BGM�̉��ʂ�ݒ�
			if (item.second.kind == SoundKind::kBgm)
			{
				ChangeVolumeSoundMem(soundVolume * m_masterVolume * item.second.maxVolume, item.second.handle);
			}
		}
	}
	//�{�C�X�̉��ʂ�ݒ�
	else if (kind == OptionSoundKind::kVoice)
	{
		m_voiceVolume = soundVolume;
		//�S�Ẳ��ʂ�ݒ�
		for (auto& item : m_utilitySoundData)
		{
			//�{�C�X�̉��ʂ�ݒ�
			if (item.second.kind == SoundKind::kVoice)
			{
				ChangeVolumeSoundMem(soundVolume * m_masterVolume * item.second.maxVolume, item.second.handle);
			}
		}
		for (auto& item : m_sceneSoundData)
		{
			//�{�C�X�̉��ʂ�ݒ�
			if (item.second.kind == SoundKind::kVoice)
			{
				ChangeVolumeSoundMem(soundVolume * m_masterVolume * item.second.maxVolume, item.second.handle);
			}
		}
	}
	//�}�X�^�[�̉��ʂ�ݒ�
	else if (kind == OptionSoundKind::kMaster)
	{
		m_masterVolume = soundVolume;
		//�S�Ẳ��ʂ�ݒ�
		for (auto& item : m_utilitySoundData)
		{
			if (item.second.kind == SoundKind::kSe)
			{
				//SE�̉��ʂ�ݒ�
				ChangeVolumeSoundMem(soundVolume * m_seVolume * item.second.maxVolume, item.second.handle);
			}
			else if (item.second.kind == SoundKind::kBgm)
			{
				//BGM�̉��ʂ�ݒ�
				ChangeVolumeSoundMem(soundVolume * m_bgmVolume * item.second.maxVolume, item.second.handle);
			}
			else if (item.second.kind == SoundKind::kVoice)
			{
				//�{�C�X�̉��ʂ�ݒ�
				ChangeVolumeSoundMem(soundVolume * m_voiceVolume * item.second.maxVolume, item.second.handle);
			}
		}
		for (auto& item : m_sceneSoundData)
		{
			if (item.second.kind == SoundKind::kSe)
			{
				//SE�̉��ʂ�ݒ�
				ChangeVolumeSoundMem(soundVolume * m_seVolume * item.second.maxVolume, item.second.handle);
			}
			else if (item.second.kind == SoundKind::kBgm)
			{
				//BGM�̉��ʂ�ݒ�
				ChangeVolumeSoundMem(soundVolume * m_bgmVolume * item.second.maxVolume, item.second.handle);
			}
			else if (item.second.kind == SoundKind::kVoice)
			{
				//�{�C�X�̉��ʂ�ݒ�
				ChangeVolumeSoundMem(soundVolume * m_voiceVolume * item.second.maxVolume, item.second.handle);
			}
		}
	}
}

float SoundManager::GetVolume(OptionSoundKind kind)
{
	//SE�̉��ʂ��擾
	if (kind == OptionSoundKind::kSe)
	{
		return m_seVolume;
	}
	//BGM�̉��ʂ��擾
	else if (kind == OptionSoundKind::kBgm)
	{
		return m_bgmVolume;
	}
	//�{�C�X�̉��ʂ��擾
	else if (kind == OptionSoundKind::kVoice)
	{
		return m_voiceVolume;
	}
	//�}�X�^�[�̉��ʂ��擾
	else if (kind == OptionSoundKind::kMaster)
	{
		return m_masterVolume;
	}
	return 0;
}
