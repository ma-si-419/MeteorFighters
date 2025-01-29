#pragma once
#include "CharacterStateBase.h"

class CharacterStateMove : public CharacterStateBase, public std::enable_shared_from_this<CharacterStateMove>
{
public:
	CharacterStateMove(std::shared_ptr<Character> character);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;
private:

	std::string m_attackKey;

	//空中に浮くかどうか
	bool m_isFloat;

	//今自分にかかっている重力
	float m_gravityPower;

	//前のフレームに地上にいたかどうか
	bool m_isLastGround;

	int m_attackButtonPushTime;

	//一度ボタンが離されたかどうか
	bool m_isReleaseX;
	bool m_isReleaseY;

};

