#pragma once
#include "CharacterStateBase.h"

class CharacterStateMove : public CharacterStateBase, public std::enable_shared_from_this<CharacterStateMove>
{
public:
	CharacterStateMove(std::shared_ptr<CharacterBase> character);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;
private:

	std::string m_attackKey;

	//�󒆂ɕ������ǂ���
	bool m_isFloat;

	//�������ɂ������Ă���d��
	float m_gravityPower;

	//�O�̃t���[���ɒn��ɂ������ǂ���
	bool m_isLastGround;

	int m_attackButtonPushTime;
};

