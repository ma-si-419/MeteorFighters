#include "SoundManager.h"
#include <map>
#include "DxLib.h"
#include "LoadCsv.h"

namespace
{
	//デフォルトのサウンドの倍率
	const float kDefaultVolume = 0.5f;
}

SoundManager::~SoundManager()
{
}

void SoundManager::Init()
{

	//音量の設定
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
		//常に使用する音声のデータ
		std::vector<std::string> utilityData;

		//Utilityのデータを取得
		if (item[static_cast<int>(FileDataSort::kFileName)] == "Utility")
		{
			std::string path = "data/sound/" + item[static_cast<int>(FileDataSort::kFileName)] + "/" + item[static_cast<int>(FileDataSort::kPath)];

			auto handle = LoadSoundMem(path.c_str());

			m_utilitySoundData[item[static_cast<int>(FileDataSort::kSoundName)]].handle = handle;
			m_utilitySoundData[item[static_cast<int>(FileDataSort::kSoundName)]].maxVolume = stof(item[static_cast<int>(FileDataSort::kMaxVolume)]);

			auto itemKind = item[static_cast<int>(FileDataSort::kSoundKind)];

			auto maxVolume = std::stof(item[static_cast<int>(FileDataSort::kMaxVolume)]);

			//音量を設定する
			//SEの場合
			if (itemKind == "SE")
			{
				ChangeVolumeSoundMem(maxVolume * m_seVolume * m_masterVolume, handle);
				m_utilitySoundData[item[static_cast<int>(FileDataSort::kSoundName)]].kind = SoundKind::kSe;
			}
			//BGMの場合
			else if (itemKind == "BGM")
			{
				ChangeVolumeSoundMem(maxVolume * m_bgmVolume * m_masterVolume, handle);
				m_utilitySoundData[item[static_cast<int>(FileDataSort::kSoundName)]].kind = SoundKind::kBgm;
			}
			//ボイスの場合
			else if (itemKind == "VOICE")
			{
				ChangeVolumeSoundMem(maxVolume * m_voiceVolume * m_masterVolume, handle);
				m_utilitySoundData[item[static_cast<int>(FileDataSort::kSoundName)]].kind = SoundKind::kVoice;
			}
		}
		//Utility以外のデータを取得
		else
		{
			//音声パスを入れるための構造体
			CsvDataInfo pushData;

			//画像の名前を保存
			pushData.name = item[static_cast<int>(FileDataSort::kSoundName)];
			//画像のファイル名を保存
			pushData.path = item[static_cast<int>(FileDataSort::kFileName)];
			pushData.path += "/";
			//画像のパスを保存
			pushData.path += item[static_cast<int>(FileDataSort::kPath)];
			//画像をどのシーンで使うかを保存
			pushData.scene = item[static_cast<int>(FileDataSort::kSceneName)];
			//音量を保存
			pushData.maxVolume = std::stof(item[static_cast<int>(FileDataSort::kMaxVolume)]);

			//音声の種類を保存
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

			//一時的にデータを保存
			pathData.push_back(pushData);

			//同じ名前のシーンが登録されていないか
			bool isSame = false;

			for (auto name : sceneName)
			{
				if (name == item[static_cast<int>(FileDataSort::kSceneName)])
				{
					isSame = true;
				}
			}
			//同じ名前が存在していなければ登録
			if (!isSame)
			{
				sceneName.push_back(item[static_cast<int>(FileDataSort::kSceneName)]);
			}
		}
	}

	//保存されたシーンの数だけ回す
	for (auto scene : sceneName)
	{
		std::vector<SoundData> pushData;
		//保存されたパスのデータの数だけ回す
		for (auto item : pathData)
		{
			//シーンの名前とパスのデータのシーンが同じであれば
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
	//音声ハンドルを削除する
	if (m_sceneSoundData.size() > 0)
	{
		for (auto& item : m_sceneSoundData)
		{
			DeleteSoundMem(item.second.handle);
		}
	}

	//指定されたシーンのロードするパス
	std::vector<SoundData> loadSoundData = m_soundData[sceneName];

	//音声をロードする
	for (auto item : loadSoundData)
	{
		std::string path = "data/sound/" + item.path;

		m_sceneSoundData[item.name].handle = LoadSoundMem(path.c_str());

		//音量を設定する

		float volume = 0;

		//SEの場合
		if (item.kind == SoundKind::kSe)
		{
			volume = item.maxVolume * m_seVolume * m_masterVolume;

			m_sceneSoundData[item.name].kind = SoundKind::kSe;
			m_sceneSoundData[item.name].maxVolume = item.maxVolume;

			ChangeVolumeSoundMem(volume, m_sceneSoundData[item.name].handle);
		}
		//BGMの場合
		else if (item.kind == SoundKind::kBgm)
		{
			volume = item.maxVolume * m_bgmVolume * m_masterVolume;

			m_sceneSoundData[item.name].kind = SoundKind::kBgm;
			m_sceneSoundData[item.name].maxVolume = item.maxVolume;

			ChangeVolumeSoundMem(volume, m_sceneSoundData[item.name].handle);
		}
		//ボイスの場合
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
	//常に使用する音声のハンドルがあればそれを再生する
	if (m_utilitySoundData.find(soundName) != m_utilitySoundData.end())
	{
		return PlaySoundMem(m_utilitySoundData[soundName].handle, DX_PLAYTYPE_BACK);
	}
	//シーンの音声ハンドルがあればそれを再生する
	else
	{
		return PlaySoundMem(m_sceneSoundData[soundName].handle, DX_PLAYTYPE_BACK);
	}

	return -1;
}

int SoundManager::PlayLoopSound(std::string soundName)
{
	int playHandle = -1;

	//常に使用する音声のハンドルがあればそれを再生する
	if (m_utilitySoundData.find(soundName) != m_utilitySoundData.end())
	{
		playHandle = PlaySoundMem(m_utilitySoundData[soundName].handle, DX_PLAYTYPE_BACK);
	}
	//シーンの音声ハンドルがあればそれを再生する
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
	//SEの音量を設定
	if (kind == OptionSoundKind::kSe)
	{
		m_seVolume = soundVolume;

		//全ての音量を設定
		for (auto& item : m_utilitySoundData)
		{
			//SEの音量を設定
			if (item.second.kind == SoundKind::kSe)
			{
				ChangeVolumeSoundMem(soundVolume * m_masterVolume * item.second.maxVolume, item.second.handle);
			}
		}
		for (auto& item : m_sceneSoundData)
		{
			//SEの音量を設定
			if (item.second.kind == SoundKind::kSe)
			{
				ChangeVolumeSoundMem(soundVolume * m_masterVolume * item.second.maxVolume, item.second.handle);
			}
		}

	}
	//BGMの音量を設定
	else if (kind == OptionSoundKind::kBgm)
	{
		m_bgmVolume = soundVolume;
		//全ての音量を設定
		for (auto& item : m_utilitySoundData)
		{
			//BGMの音量を設定
			if (item.second.kind == SoundKind::kBgm)
			{
				ChangeVolumeSoundMem(soundVolume * m_masterVolume * item.second.maxVolume, item.second.handle);
			}
		}
		for (auto& item : m_sceneSoundData)
		{
			//BGMの音量を設定
			if (item.second.kind == SoundKind::kBgm)
			{
				ChangeVolumeSoundMem(soundVolume * m_masterVolume * item.second.maxVolume, item.second.handle);
			}
		}
	}
	//ボイスの音量を設定
	else if (kind == OptionSoundKind::kVoice)
	{
		m_voiceVolume = soundVolume;
		//全ての音量を設定
		for (auto& item : m_utilitySoundData)
		{
			//ボイスの音量を設定
			if (item.second.kind == SoundKind::kVoice)
			{
				ChangeVolumeSoundMem(soundVolume * m_masterVolume * item.second.maxVolume, item.second.handle);
			}
		}
		for (auto& item : m_sceneSoundData)
		{
			//ボイスの音量を設定
			if (item.second.kind == SoundKind::kVoice)
			{
				ChangeVolumeSoundMem(soundVolume * m_masterVolume * item.second.maxVolume, item.second.handle);
			}
		}
	}
	//マスターの音量を設定
	else if (kind == OptionSoundKind::kMaster)
	{
		m_masterVolume = soundVolume;
		//全ての音量を設定
		for (auto& item : m_utilitySoundData)
		{
			if (item.second.kind == SoundKind::kSe)
			{
				//SEの音量を設定
				ChangeVolumeSoundMem(soundVolume * m_seVolume * item.second.maxVolume, item.second.handle);
			}
			else if (item.second.kind == SoundKind::kBgm)
			{
				//BGMの音量を設定
				ChangeVolumeSoundMem(soundVolume * m_bgmVolume * item.second.maxVolume, item.second.handle);
			}
			else if (item.second.kind == SoundKind::kVoice)
			{
				//ボイスの音量を設定
				ChangeVolumeSoundMem(soundVolume * m_voiceVolume * item.second.maxVolume, item.second.handle);
			}
		}
		for (auto& item : m_sceneSoundData)
		{
			if (item.second.kind == SoundKind::kSe)
			{
				//SEの音量を設定
				ChangeVolumeSoundMem(soundVolume * m_seVolume * item.second.maxVolume, item.second.handle);
			}
			else if (item.second.kind == SoundKind::kBgm)
			{
				//BGMの音量を設定
				ChangeVolumeSoundMem(soundVolume * m_bgmVolume * item.second.maxVolume, item.second.handle);
			}
			else if (item.second.kind == SoundKind::kVoice)
			{
				//ボイスの音量を設定
				ChangeVolumeSoundMem(soundVolume * m_voiceVolume * item.second.maxVolume, item.second.handle);
			}
		}
	}
}

float SoundManager::GetVolume(OptionSoundKind kind)
{
	//SEの音量を取得
	if (kind == OptionSoundKind::kSe)
	{
		return m_seVolume;
	}
	//BGMの音量を取得
	else if (kind == OptionSoundKind::kBgm)
	{
		return m_bgmVolume;
	}
	//ボイスの音量を取得
	else if (kind == OptionSoundKind::kVoice)
	{
		return m_voiceVolume;
	}
	//マスターの音量を取得
	else if (kind == OptionSoundKind::kMaster)
	{
		return m_masterVolume;
	}
	return 0;
}
