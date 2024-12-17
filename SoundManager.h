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
	/// ������
	/// </summary>
	void Init();

	/// <summary>
	/// �V�[���̉����n���h�������[�h����
	/// </summary>
	/// <param name="">�V�[���̖��O</param>
	void LoadSceneSound(std::string sceneName);

	int OncePlaySound(std::string soundName);
	
	int LoopPlaySound(std::string soundName);

	void StopLoopSound(int playHandle);
private:

	//���ׂẲ����̃p�X
	std::map<std::string, std::vector<std::pair<std::string, std::string>>> m_pathData;

	//������V�[���̉����̃n���h��
	std::map<std::string, int> m_sceneSoundHandle;

	//���[�v�Đ����̉����̃v���C�n���h��
	std::vector<int> m_loopPlayHandles;

};