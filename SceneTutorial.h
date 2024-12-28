#pragma once
#include "SceneBase.h"
#include <vector>

class GameManagerBase;
class Character;
class SceneTutorial : public SceneBase
{
public:

	SceneTutorial(SceneManager& sceneManager);

	virtual ~SceneTutorial();

	//����������
	virtual void Init() override;
	//�X�V����
	virtual void Update() override;
	//�`�揈��
	virtual void Draw() override;
	//�I������
	virtual void End() override;

private:

	std::vector<std::shared_ptr<Character>> m_pCharacters;

	std::shared_ptr<GameManagerBase> m_pManager;

};

