#pragma once
#include <list>
#include <vector>
#include <memory>
#include "Collidable.h"

class Physics final
{
public:
	//判定をする衝突物を登録・削除する
	void Entry(std::shared_ptr<Collidable> col);
	void Exit(std::shared_ptr<Collidable> col);
	//登録した衝突物の物理移動、衝突通知を行う
	void Update();
	//当たり判定の表示を行う
	void DebugDraw();
private:
	std::list<std::shared_ptr<Collidable>> m_collidables; //登録されたcollidableのリスト
	void FixPosition();

	bool CheckCollide(std::shared_ptr<Collidable> first, std::shared_ptr<Collidable> second);

	// OnCollideの衝突通知のためのデータ
	struct OnCollideInfo
	{
		std::shared_ptr<Collidable> owner;
		std::shared_ptr<Collidable> colider;
		void OnCollide() { owner->OnCollide(colider); }
	};
};

