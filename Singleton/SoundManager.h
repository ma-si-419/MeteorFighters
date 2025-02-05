#pragma once
#include <string>
#include <vector>
#include <map>
class SoundManager
{
public:

	enum class OptionSoundKind
	{
		kMaster,
		kSe,
		kBgm,
		kVoice,
		kOptionSoundKindNum
	};

private:

	enum class FileDataSort
	{
		kSoundName,
		kFileName,
		kPath,
		kSceneName,
		kMaxVolume,
		kSoundKind
	};


	enum class SoundKind
	{
		kBgm,
		kSe,
		kVoice
	};

	struct SoundData
	{
		std::string name;
		std::string path;
		float maxVolume;
		SoundKind kind;
	};

	struct HandleData
	{
		SoundKind kind;
		int handle;
		float maxVolume;
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

	void StopSound(std::string soundName);

	void StopLoopSound(std::string soundName);

	bool IsPlayingSound(std::string soundName);

	/// <summary>
	/// サウンドの音量を設定する
	/// </summary>
	/// <param name="kind">サウンドの種類</param>
	/// <param name="soundVolume">サウンドのボリューム(0.0 ~ 1.0)</param>
	void SetVolume(OptionSoundKind kind, float soundVolume);

	/// <summary>
	/// サウンドの音量を取得する
	/// </summary>
	/// <param name="kind">サウンドの種類</param>
	/// <returns>サウンドのボリューム</returns>
	float GetVolume(OptionSoundKind kind);

private:

	//外部から持ってきた音声のパスを保存する
	std::map<std::string, std::vector<SoundData>> m_soundData;

	//今いるシーンの音声のハンドル
	std::map<std::string, HandleData> m_sceneSoundData;

	//常に使用する音声のハンドル
	std::map<std::string, HandleData> m_utilitySoundData;

	//SEの音量
	float m_seVolume = 0;

	//BGMの音量
	float m_bgmVolume = 0;

	//ボイスの音量
	float m_voiceVolume = 0;

	//マスターの音量
	float m_masterVolume = 0;
};