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

	void StopSound(std::string soundName);

	void StopLoopSound(std::string soundName);

	bool IsPlayingSound(std::string soundName);

	/// <summary>
	/// �T�E���h�̉��ʂ�ݒ肷��
	/// </summary>
	/// <param name="kind">�T�E���h�̎��</param>
	/// <param name="soundVolume">�T�E���h�̃{�����[��(0.0 ~ 1.0)</param>
	void SetVolume(OptionSoundKind kind, float soundVolume);

	/// <summary>
	/// �T�E���h�̉��ʂ��擾����
	/// </summary>
	/// <param name="kind">�T�E���h�̎��</param>
	/// <returns>�T�E���h�̃{�����[��</returns>
	float GetVolume(OptionSoundKind kind);

private:

	//�O�����玝���Ă��������̃p�X��ۑ�����
	std::map<std::string, std::vector<SoundData>> m_soundData;

	//������V�[���̉����̃n���h��
	std::map<std::string, HandleData> m_sceneSoundData;

	//��Ɏg�p���鉹���̃n���h��
	std::map<std::string, HandleData> m_utilitySoundData;

	//SE�̉���
	float m_seVolume = 0;

	//BGM�̉���
	float m_bgmVolume = 0;

	//�{�C�X�̉���
	float m_voiceVolume = 0;

	//�}�X�^�[�̉���
	float m_masterVolume = 0;
};