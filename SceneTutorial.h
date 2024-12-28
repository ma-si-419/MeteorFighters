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

	//‰Šú‰»ˆ—
	virtual void Init() override;
	//XVˆ—
	virtual void Update() override;
	//•`‰æˆ—
	virtual void Draw() override;
	//I—¹ˆ—
	virtual void End() override;

private:

	std::vector<std::shared_ptr<Character>> m_pCharacters;

	std::shared_ptr<GameManagerBase> m_pManager;

};

