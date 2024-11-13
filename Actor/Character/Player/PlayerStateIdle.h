#pragma once
#include "PlayerStateBase.h"
class PlayerStateIdle : public PlayerStateBase , public std::enable_shared_from_this<PlayerStateIdle>
{
public:
	PlayerStateIdle(std::shared_ptr<Player> player);

	/// <summary>
	/// �X�e�[�g�̏I���A�j���[�V�������Đ�����(�u�����h�̑����Œ�)
	/// </summary>
	/// <param name="kind">�Đ�����A�j���[�V�����ԍ�</param>
	/// <param name="time">�Đ����鎞��</param>
	void SetEndAnim(int kind,int time);

	/// <summary>
	/// �X�e�[�g�̏I���A�j���[�V�������Đ�����(�u�����h�̑�����)
	/// </summary>
	/// <param name="kind">�Đ�����A�j���[�V�����ԍ�</param>
	/// <param name="time">�Đ����鎞��</param>
	/// <param name="blendSpeed">�A�j���[�V�����u�����h�̑���</param>
	void SetEndAnim(int kind,int time,float blendSpeed);

	virtual void Enter() override;

	virtual void Update() override;
	
	virtual void Exit() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

private:

	std::string m_attackKey;

	int m_attackButtonPushTime;

	bool m_isPlayEndAnim;

	int m_endAnimTime;
};

