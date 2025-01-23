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
	/// 更新を止める
	/// </summary>
	void StopUpdate() { m_isUpdate = false; }

	/// <summary>
	/// 更新を始める
	/// </summary>
	void StartUpdate() { m_isUpdate = true; }

	/// <summary>
	/// ステージの当たり判定の更新を行う
	/// </summary>
	void StageColUpdate();

	/// <summary>
	/// 当たり判定を行うステージモデルをセットする
	/// </summary>
	void SetStage(int stageHandle);

	/// <summary>
	/// 当たり判定を行うステージモデルを削除する
	/// </summary>
	void DeleteStage() { m_stageHandle = -1; }

	/// <summary>
	/// 当たり判定の表示を行う
	/// </summary>
	void DebugDraw();

	/// <summary>
	/// 登録していないけど当たり判定を取りたいものと登録している当たり判定がぶつかっているか取得する
	/// </summary>
	/// <param name="pos">登録していない当たり判定の座標</param>
	/// <param name="collider">登録していない当たり判定の情報</param>
	/// <param name="tag">何とぶつかったらtrueを返すか</param>
	/// <returns>指定したタグの当たり判定とぶつかったかどうか</returns>
	bool GetHitObject(MyEngine::Vector3 pos, std::shared_ptr<ColliderData> collider, ObjectTag tag);
	

private:

	/// <summary>
	/// 座標を確定する
	/// </summary>
	void ConfirmPosition();

	/// <summary>
	/// 次の座標を計算する
	/// </summary>
	/// <param name="hitCol">衝突したもの</param>
	void FixNextPosition(OnCollideInfo hitCol);

	//チェックしたポリゴンが壁ポリゴンか床ポリゴンかを判断し保存する
	void CheckWallAndFloor(std::shared_ptr<Collidable> collider);
	//壁ポリゴンとの当たり判定をチェックし、移動させる
	void FixPositionWithWall(std::shared_ptr<Collidable> collider);
	void FixPositionWithWallInternal(std::shared_ptr<Collidable> collider);
	//床ポリゴンとの当たり判定をチェックし、移動させる
	void FixNowPositionWithFloor(std::shared_ptr<Collidable> collider);

	bool IsCheckCollide(std::shared_ptr<Collidable> first, std::shared_ptr<Collidable> second);
private:

	std::list<std::shared_ptr<Collidable>> m_collidables; //登録されたcollidableのリスト		
	//当たり判定結果構造体
	MV1_COLL_RESULT_POLY_DIM m_hitDim{};
	//ポリゴンの構造体にアクセスするために使用するポインタ
	MV1_COLL_RESULT_POLY* m_pPoly = nullptr;
	//線分とポリゴンとの当たり判定の結果を代入する構造体
	HITRESULT_LINE m_lineRes{};
	//ステージのモデルハンドル
	int m_stageHandle = -1;

	//更新をするかどうか
	bool m_isUpdate = true;

};