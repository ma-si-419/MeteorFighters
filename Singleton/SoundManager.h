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
		kFileName,
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

	int PlayOnceSound(std::string soundName);
	
	int PlayLoopSound(std::string soundName);

	void StopLoopSound(std::string soundName);

	bool IsPlayingSound(std::string soundName);

	/// <summary>
	/// 音声のボリュームを設定する
	/// </summary>
	/// <param name="soundName">音声の名前</param>
	/// <param name="volume">大きさ</param>
	void SetSoundVolume(std::string soundName,int volume);
private:

	//すべての音声のパス
	std::map<std::string, std::vector<std::pair<std::string, std::string>>> m_pathData;

	//今いるシーンの音声のハンドル
	std::map<std::string, int> m_sceneSoundHandle;

	//常に使用する音声のハンドル
	std::map<std::string, int> m_utilitySoundHandle;


};