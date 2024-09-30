#pragma once
#include "Actor.h"
class SceneGame;
class CharacterBase : public Actor
{
public: 
	CharacterBase(ObjectTag tag);
	~CharacterBase();
protected:
	//モデルハンドル
	int m_modelHandle;
	//シーンのポインタ
	std::shared_ptr<SceneGame> m_pScene;
};