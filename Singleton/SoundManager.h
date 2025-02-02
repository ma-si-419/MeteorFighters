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
	/// ������
	/// </summary>
	void Init();

	/// <summary>
	/// �V�[���̉����n���h�������[�h����
	/// </summary>
	/// <param name="">�V�[���̖��O</param>
	void LoadSceneSound(std::string sceneName);

	int PlayOnceSound(std::string soundName);
	
	int PlayLoopSound(std::string soundName);

	void StopLoopSound(std::string soundName);

	bool IsPlayingSound(std::string soundName);

	/// <summary>
	/// �����̃{�����[����ݒ肷��
	/// </summary>
	/// <param name="soundName">�����̖��O</param>
	/// <param name="volume">�傫��</param>
	void SetSoundVolume(std::string soundName,int volume);
private:

	//���ׂẲ����̃p�X
	std::map<std::string, std::vector<std::pair<std::string, std::string>>> m_pathData;

	//������V�[���̉����̃n���h��
	std::map<std::string, int> m_sceneSoundHandle;

	//��Ɏg�p���鉹���̃n���h��
	std::map<std::string, int> m_utilitySoundHandle;


};