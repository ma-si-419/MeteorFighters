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
	/// �����蔻��̕\�����s��
	/// </summary>
	void DebugDraw();
private:
	std::list<std::shared_ptr<Collidable>> m_collidables; //�o�^���ꂽcollidable�̃��X�g

	/// <summary>
	/// ���W���m�肷��
	/// </summary>
	void ConfirmPosition();

	/// <summary>
	/// ���̍��W���v�Z����
	/// </summary>
	/// <param name="hitCol">�Փ˂�������</param>
	void FixNextPosition(OnCollideInfo hitCol);

	bool IsCheckCollide(std::shared_ptr<Collidable> first, std::shared_ptr<Collidable> second);
};