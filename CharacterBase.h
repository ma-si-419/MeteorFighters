#pragma once
#include "Actor.h"
class SceneGame;
class CharacterBase : public Actor
{
public: 
	CharacterBase(ObjectTag tag);
	~CharacterBase();
protected:
	//���f���n���h��
	int m_modelHandle;
	//�V�[���̃|�C���^
	std::shared_ptr<SceneGame> m_pScene;
};