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

	// OnCollideの衝突通知のためのデータ
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
	/// 判定をする衝突物を登録
	/// </summary>
	/// <param name="col">登録するCollidable</param>
	void Entry(std::shared_ptr<Collidable> col);
	/// <summary>
	/// 判定している衝突物を削除する
	/// </summary>
	/// <param name="col">登録するCollidable</param>
	void Exit(std::shared_ptr<Collidable> col);
	/// <summary>
	/// 登録した衝突物の物理移動、衝突通知を行う
	/// </summary>
	void Update();
	/// <summary>
	/// 当たり判定の表示を行う
	/// </summary>
	void DebugDraw();
private:
	std::list<std::shared_ptr<Collidable>> m_collidables; //登録されたcollidableのリスト

	/// <summary>
	/// 座標を確定する
	/// </summary>
	void ConfirmPosition();

	/// <summary>
	/// 次の座標を計算する
	/// </summary>
	/// <param name="hitCol">衝突したもの</param>
	void FixNextPosition(OnCollideInfo hitCol);

	bool IsCheckCollide(std::shared_ptr<Collidable> first, std::shared_ptr<Collidable> second);
};