#pragma once
#include "CharacterStateBase.h"
#include "LocalPos.h"

class Effect;
class CharacterStateRush : public CharacterStateBase,public std::enable_shared_from_this<CharacterStateRush>
{
public:
	CharacterStateRush(std::shared_ptr<Character> character);

	virtual void SetMoveDir(MyEngine::Vector3 dir);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

private:

	//�ړ������ɂ���x�N�g��(�����Ɍ������Ĉړ������悤�Ƃ���)
	MyEngine::Vector3 m_moveTarget;

	//�ړ������x�N�g��
	MyEngine::Vector3 m_moveDir;

	//�G�̌��Ɍ������ēˌ����邩�ǂ���
	bool m_isRushEnemy;

	//�h���S���_�b�V���̖ړI���W
	MyEngine::Vector3 m_rushTargetPos;

	//�h���S���_�b�V�����I��������ǂ���
	bool m_isEndRush;

	//�h���S���_�b�V�����I����ĉ��t���[����������
	int m_rushEndTime;

	//�U�����͂��ꂽ���ǂ���
	bool m_isAttackInput;

};

