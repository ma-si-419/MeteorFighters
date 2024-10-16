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
public:

	static Physics& GetInstance()
	{
		static Physics instance;
		return instance;
	}
	//���������Փ˕���o�^�E�폜����
	void Entry(std::shared_ptr<Collidable> col);
	void Exit(std::shared_ptr<Collidable> col);
	//�o�^�����Փ˕��̕����ړ��A�Փ˒ʒm���s��
	void Update();
	//�����蔻��̕\�����s��
	void DebugDraw();
private:
	std::list<std::shared_ptr<Collidable>> m_collidables; //�o�^���ꂽcollidable�̃��X�g
	void FixPosition();

	bool IsCheckCollide(std::shared_ptr<Collidable> first, std::shared_ptr<Collidable> second);

	// OnCollide�̏Փ˒ʒm�̂��߂̃f�[�^
	struct OnCollideInfo
	{
		std::shared_ptr<Collidable> owner;
		std::shared_ptr<Collidable> colider;
		void OnCollide() { owner->OnCollide(colider); }
	};
};

