#pragma once
#include "Actor.h"
class SceneGame;
class GameManager;
class CharacterStateBase;
class CharacterBase : public Actor
{
public: 

	enum class AnimKind
	{
		kIdle,
		kGuard,
		kStep,
		kMove
	};


	CharacterBase(ObjectTag tag);
	~CharacterBase();

	void SetGameManager(std::shared_ptr<GameManager> manager);

	MyEngine::Vector3 GetPos();

	void ChangeAnim(int animNumber,bool loop);

	void PlayAnim();

	bool GetAnimEnd();
protected:
	//���f���n���h��
	int m_modelHandle;
	//�V�[���̃|�C���^
	std::shared_ptr<SceneGame> m_pScene;
	//�}�l�[�W���[�̃|�C���^
	std::shared_ptr<GameManager> m_pGameManager;
	//State
	std::shared_ptr<CharacterStateBase> m_pState;
	//�A�j���[�V�����̏��
	int m_attachAnim;
	int m_totalAnimTime;
	int m_playAnimTime;
	float m_animPlaySpeed;
	bool m_isLoop;

};