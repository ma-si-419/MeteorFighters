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
private:
	std::vector<std::shared_ptr<Actor>> m_pActors;
	std::shared_ptr<GameManager> m_pGameManager;
};

