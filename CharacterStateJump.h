#pragma once
#include "CharacterStateBase.h"
class CharacterStateJump : public CharacterStateBase,public std::enable_shared_from_this<CharacterStateJump>
{
public:

	CharacterStateJump(std::shared_ptr<CharacterBase> character);

	/// <summary>
	/// ���E�O��ړ��x�N�g���̂���W�����v���s���Ƃ��ɌĂ�
	/// </summary>
	/// <param name="moveVec">���̈ړ��x�N�g��</param>
	void StartJump(MyEngine::Vector3 moveVec);

	/// <summary>
	/// ���E�O��ړ����Ȃ��W�����v���s�����ɌĂ�
	/// </summary>
	void StartJump();

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

private:

	std::string m_attackKey;

	int m_attackButtonPushTime;

	MyEngine::Vector3 m_moveVec;

};
