#pragma once
#include "SceneBase.h"
#include <vector>
class GameManager;
class Actor;
class SceneGame : public SceneBase
{
public:
	SceneGame(SceneManager& sceneManager);
	virtual ~SceneGame();
	//����������
	virtual void Init();
	//�X�V����
	virtual void Update();
	//�`�揈��
	virtual void Draw();
	//�I������
	virtual void End();

	/// <summary>
	/// �g���L�����N�^�[���Z�b�g����(SceneSelect����SceneGame�Ɉڍs����O�ɕK���Ă�)
	/// </summary>
	/// <param name="player">�v���C���[���g���L�����N�^�[�ԍ�</param>
	/// <param name="enemy">�G�l�~�[���g�p����L�����N�^�[�ԍ�</param>
	void SetCharacter(int player,int enemy);

private:
	std::vector<std::shared_ptr<Actor>> m_pActors;
	std::shared_ptr<GameManager> m_pGameManager;

};

