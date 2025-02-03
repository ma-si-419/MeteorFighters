#pragma once
#include "SceneBase.h"
#include <vector>
#include <string>

class GameManagerBase;
class Character;
class BattleUi;
class SceneGame : public SceneBase
{
public:
	SceneGame(SceneManager& sceneManager);
	virtual ~SceneGame();
	//����������
	virtual void Init() override;
	//�X�V����
	virtual void Update() override;
	//�񓯊��������s���Ă���Ƃ��̃A�b�v�f�[�g
	virtual void UpdateAsyncLoad() override;
	//�`�揈��
	virtual void Draw() override;
	//�I������
	virtual void End() override;

	/// <summary>
	/// �g���L�����N�^�[���Z�b�g����(SceneSelect����SceneGame�Ɉڍs����O�ɕK���Ă�)
	/// </summary>
	/// <param name="player">�v���C���[���g���L�����N�^�[�ԍ�</param>
	/// <param name="enemy">�G�l�~�[���g�p����L�����N�^�[�ԍ�</param>
	/// <param name="enemyLevel">�G�l�~�[�̃��x��</param>
	void SetCharacter(int player,int enemy,int enemyLevel);

	/// <summary>
	/// �o�g�����Ɏg�p����BGM��ݒ肷��
	/// </summary>
	/// <param name="musicName">BGM�̖��O</param>
	void SetMusicName(std::string musicName);

private:
	std::vector<std::shared_ptr<Character>> m_pCharacters;

	std::shared_ptr<GameManagerBase> m_pGameManager;
};

