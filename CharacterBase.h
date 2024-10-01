#pragma once
#include "Actor.h"
class SceneGame;
class GameManager;
class CharacterBase : public Actor
{
public: 
	CharacterBase(ObjectTag tag);
	~CharacterBase();

	void SetGameManager(std::shared_ptr<GameManager> manager);

	MyEngine::Vector3 GetPos();

protected:
	//���f���n���h��
	int m_modelHandle;
	//�V�[���̃|�C���^
	std::shared_ptr<SceneGame> m_pScene;
	//�}�l�[�W���[�̃|�C���^
	std::shared_ptr<GameManager> m_pGameManager;
};