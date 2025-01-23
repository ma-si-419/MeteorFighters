#pragma once
#include <list>
#include <vector>
#include <memory>
#include "Collidable.h"

class Physics final
{
private:
	Physics() = default;
	~Physics();
	Physics(const Physics&) = delete;
	Physics& operator = (const Physics&) = delete;
	Physics(Physics&&) = delete;
	Physics& operator = (Physics&&) = delete;

private:

	// OnCollide�̏Փ˒ʒm�̂��߂̃f�[�^
	struct OnCollideInfo
	{
		std::shared_ptr<Collidable> owner;
		std::shared_ptr<Collidable> colider;
		void OnCollide() { owner->OnCollide(colider); }
	};

public:
	static Physics& GetInstance()
	{
		static Physics instance;
		return instance;
	}
	/// <summary>
	/// ���������Փ˕���o�^
	/// </summary>
	/// <param name="col">�o�^����Collidable</param>
	void Entry(std::shared_ptr<Collidable> col);
	/// <summary>
	/// ���肵�Ă���Փ˕����폜����
	/// </summary>
	/// <param name="col">�o�^����Collidable</param>
	void Exit(std::shared_ptr<Collidable> col);
	/// <summary>
	/// �o�^�����Փ˕��̕����ړ��A�Փ˒ʒm���s��
	/// </summary>
	void Update();

	/// <summary>
	/// �X�V���~�߂�
	/// </summary>
	void StopUpdate() { m_isUpdate = false; }

	/// <summary>
	/// �X�V���n�߂�
	/// </summary>
	void StartUpdate() { m_isUpdate = true; }

	/// <summary>
	/// �X�e�[�W�̓����蔻��̍X�V���s��
	/// </summary>
	void StageColUpdate();

	/// <summary>
	/// �����蔻����s���X�e�[�W���f�����Z�b�g����
	/// </summary>
	void SetStage(int stageHandle);

	/// <summary>
	/// �����蔻����s���X�e�[�W���f�����폜����
	/// </summary>
	void DeleteStage() { m_stageHandle = -1; }

	/// <summary>
	/// �����蔻��̕\�����s��
	/// </summary>
	void DebugDraw();

	/// <summary>
	/// �o�^���Ă��Ȃ����Ǔ����蔻�����肽�����̂Ɠo�^���Ă��铖���蔻�肪�Ԃ����Ă��邩�擾����
	/// </summary>
	/// <param name="pos">�o�^���Ă��Ȃ������蔻��̍��W</param>
	/// <param name="collider">�o�^���Ă��Ȃ������蔻��̏��</param>
	/// <param name="tag">���ƂԂ�������true��Ԃ���</param>
	/// <returns>�w�肵���^�O�̓����蔻��ƂԂ��������ǂ���</returns>
	bool GetHitObject(MyEngine::Vector3 pos, std::shared_ptr<ColliderData> collider, ObjectTag tag);
	

private:

	/// <summary>
	/// ���W���m�肷��
	/// </summary>
	void ConfirmPosition();

	/// <summary>
	/// ���̍��W���v�Z����
	/// </summary>
	/// <param name="hitCol">�Փ˂�������</param>
	void FixNextPosition(OnCollideInfo hitCol);

	//�`�F�b�N�����|���S�����ǃ|���S�������|���S�����𔻒f���ۑ�����
	void CheckWallAndFloor(std::shared_ptr<Collidable> collider);
	//�ǃ|���S���Ƃ̓����蔻����`�F�b�N���A�ړ�������
	void FixPositionWithWall(std::shared_ptr<Collidable> collider);
	void FixPositionWithWallInternal(std::shared_ptr<Collidable> collider);
	//���|���S���Ƃ̓����蔻����`�F�b�N���A�ړ�������
	void FixNowPositionWithFloor(std::shared_ptr<Collidable> collider);

	bool IsCheckCollide(std::shared_ptr<Collidable> first, std::shared_ptr<Collidable> second);
private:

	std::list<std::shared_ptr<Collidable>> m_collidables; //�o�^���ꂽcollidable�̃��X�g		
	//�����蔻�茋�ʍ\����
	MV1_COLL_RESULT_POLY_DIM m_hitDim{};
	//�|���S���̍\���̂ɃA�N�Z�X���邽�߂Ɏg�p����|�C���^
	MV1_COLL_RESULT_POLY* m_pPoly = nullptr;
	//�����ƃ|���S���Ƃ̓����蔻��̌��ʂ�������\����
	HITRESULT_LINE m_lineRes{};
	//�X�e�[�W�̃��f���n���h��
	int m_stageHandle = -1;

	//�X�V�����邩�ǂ���
	bool m_isUpdate = true;

};