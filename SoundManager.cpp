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

		//画像の名前を保存
		pushData.name = item[static_cast<int>(FileDataSort::kSoundName)];
		//画像のパスを保存
		pushData.path = item[static_cast<int>(FileDataSort::kPath)];
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
	//画像ハンドルを削除する
	if (m_sceneSoundHandle.size() > 0)
	{
		for (auto& item : m_sceneSoundHandle)
		{
			DeleteSoundMem(item.second);
		}
	}

	//指定されたシーンのロードするパス
	std::vector<std::pair<std::string, std::string>> loadPaths = m_pathData[sceneName];

	//画像をロードする
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

	//配列から削除する
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
