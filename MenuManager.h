#pragma once
#include <memory>

class MenuUi;
class MenuManager
{
public:
	MenuManager();
	
	~MenuManager();
	
	void Init();

	void Update();

	void Draw();

	int GetNextScene() { return m_nextScene; }

private:

	std::shared_ptr<MenuUi> m_pMenuUi;

	int m_nextScene;
};