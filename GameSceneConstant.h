#pragma once
/// <summary>
/// �Q�[���V�[���ŋ��ʂň����萔
/// </summary>
namespace GameSceneConstant
{
	//�W�����v��
	constexpr float kJumpPower = 6.0f;
	//�󒆂ł̏d�͂̋���
	constexpr float kSkyGravityPower = -0.2f;
	//�n��ł̏d�͂̋���
	constexpr float kGroundGravityPower = -0.5f;
	//�������x�̌��E
	constexpr float kMaxFallSpeed = -3.5f;
	//�`���[�W�U���Ɣ��肷�钷�����̎���
	constexpr int kChargeAttackTime = 15;
	//�_�b�V���Ɏg���C��
	constexpr int kDashCost = 20;
	//�G�Ƃ̋������߂��Ɣ��肷�鋗��
	constexpr float kNearLange = 75.0f;
	//���f���̑傫��
	constexpr float kModelScale = 0.1f;
	//�c���̏��������x
	constexpr float kAfterImageInitOpacityRate = 3.0f;
	//�c���̈�ԔZ�������x
	constexpr float kAfterImageMaxOpacityRate = 0.65f;
	//�c�����쐬���銴�o
	constexpr int kAfterImageCreateTime = 6;
	//�c������������
	constexpr float kAfterImageDeleteSpeed = kAfterImageInitOpacityRate / 10;
}