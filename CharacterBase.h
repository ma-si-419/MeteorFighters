#pragma once
#include "Actor.h"
class SceneGame;
class GameManager;
class CharacterStateBase;
class CharacterBase : public Actor
{
public: 
	CharacterBase(ObjectTag tag);
	~CharacterBase();

	void SetGameManager(std::shared_ptr<GameManager> manager);

	MyEngine::Vector3 GetPos();

protected:
	//モデルハンドル
	int m_modelHandle;
	//シーンのポインタ
	std::shared_ptr<SceneGame> m_pScene;
	//マネージャーのポインタ
	std::shared_ptr<GameManager> m_pGameManager;
	//State
	std::shared_ptr<CharacterStateBase> m_pState;
};