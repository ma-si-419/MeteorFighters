#pragma once
#include <string>
#include <vector>
#include <map>
class SoundManager
{
private:

	enum class FileDataSort
	{
		kSoundName,
		kPath,
		kSceneName
	};

private:
	SoundManager() = default;
	~SoundManager();
	SoundManager(const SoundManager&) = delete;
	SoundManager& operator = (const SoundManager&) = delete;
	SoundManager(SoundManager&&) = delete;
	SoundManager& operator = (SoundManager&&) = delete;
public:

	static SoundManager& GetInstance()
	{
		static SoundManager instance;
		return instance;
	}

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// シーンの音声ハンドルをロードする
	/// </summary>
	/// <param name="">シーンの名前</param>
	void LoadSceneSound(std::string sceneName);

	int OncePlaySound(std::string soundName);
	
	int LoopPlaySound(std::string soundName);

	void StopLoopSound(int playHandle);
private:

	//すべての音声のパス
	std::map<std::string, std::vector<std::pair<std::string, std::string>>> m_pathData;

	//今いるシーンの音声のハンドル
	std::map<std::string, int> m_sceneSoundHandle;

	//ループ再生中の音声のプレイハンドル
	std::vector<int> m_loopPlayHandles;

};