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
		//常に使用する音声のデータ
		std::vector<std::string> utilityData;

		//Utilityのデータを取得
		if (item[static_cast<int>(FileDataSort::kFileName)] == "Utility")
		{
			std::string path = "data/sound/" + item[static_cast<int>(FileDataSort::kFileName)] + "/" + item[static_cast<int>(FileDataSort::kPath)];

			m_utilitySoundHandle[item[static_cast<int>(FileDataSort::kSoundName)]] = LoadSoundMem(path.c_str());
		}
		//Utility以外のデータを取得
		else
		{
			//音声パスを入れるための構造体
			pushDataInfo pushData;

			//画像の名前を保存
			pushData.name = item[static_cast<int>(FileDataSort::kSoundName)];
			//画像のファイル名を保存
			pushData.path = item[static_cast<int>(FileDataSort::kFileName)];
			pushData.path += "/";
			//画像のパスを保存
			pushData.path += item[static_cast<int>(FileDataSort::kPath)];
			//画像をどのシーンで使うかを保存
			pushData.scene = item[static_cast<int>(FileDataSort::kSceneName)];

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
		std::vector<std::pair<std::string, std::string>> pushData;
		//保存されたパスのデータの数だけ回す
		for (auto item : pathData)
		{
			//シーンの名前とパスのデータのシーンが同じであれば
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
	//音声ハンドルを削除する
	if (m_sceneSoundHandle.size() > 0)
	{
		for (auto& item : m_sceneSoundHandle)
		{
			DeleteSoundMem(item.second);
		}
	}

	//指定されたシーンのロードするパス
	std::vector<std::pair<std::string, std::string>> loadPaths = m_pathData[sceneName];

	//音声をロードする
	for (auto item : loadPaths)
	{
		std::string path = "data/sound/" + item.second;

		m_sceneSoundHandle[item.first] = LoadSoundMem(path.c_str());

		//音量を設定する
		ChangeVolumeSoundMem(kDefaultVolume, m_sceneSoundHandle[item.first]);
	}
}

int SoundManager::PlayOnceSound(std::string soundName)
{
	//常に使用する音声のハンドルがあればそれを再生する
	if (m_utilitySoundHandle.find(soundName) != m_utilitySoundHandle.end())
	{
		return PlaySoundMem(m_utilitySoundHandle[soundName], DX_PLAYTYPE_BACK);
	}
	//シーンの音声ハンドルがあればそれを再生する
	else
	{
		return PlaySoundMem(m_sceneSoundHandle[soundName], DX_PLAYTYPE_BACK);
	}

	return -1;
}

int SoundManager::PlayLoopSound(std::string soundName)
{
	int playHandle = -1;

	//常に使用する音声のハンドルがあればそれを再生する
	if (m_utilitySoundHandle.find(soundName) != m_utilitySoundHandle.end())
	{
		playHandle = PlaySoundMem(m_utilitySoundHandle[soundName], DX_PLAYTYPE_BACK);
	}
	//シーンの音声ハンドルがあればそれを再生する
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
