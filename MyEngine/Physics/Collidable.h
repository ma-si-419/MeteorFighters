#pragma once
#include "ObjectTag.h"
#include "ColliderData.h"
#include "Rigidbody.h"
#include <memory>
namespace
{
	constexpr int kMaxColHitPolyNum = 200;
}

class Physics;
class Collidable abstract : public std::enable_shared_from_this<Collidable>
{
public:
	Collidable(ObjectTag tag, ColliderData::Kind colKind);
	virtual ~Collidable();
	virtual void Init();
	virtual void Final();

	/// <summary>
	/// 自身のタグを返す
	/// </summary>
	/// <returns>自身のObjectTag</returns>
	ObjectTag GetTag() const { return m_tag; }


	/// <summary>
	/// Collidableどうしがぶつかった時の処理を書く
	/// </summary>
	/// <param name="collider">ぶつかった相手側のCollidable</param>
	virtual void OnCollide(std::shared_ptr<Collidable> collider) abstract;

protected:
	//物理データ
	Rigidbody m_rigidbody;
	//当たり判定データ
	std::shared_ptr<ColliderData> m_pColData;
private:
	std::shared_ptr<ColliderData> CreateColliderData(ColliderData::Kind kind);

	ObjectTag m_tag;

	MyEngine::Vector3 m_nextPos;

	//衝突した時に物理挙動をしない時にtrueにする
	bool m_isTrigger;

	//移動してるかどうか
	bool m_isMove;

	//ステージのポリゴンとぶつかったかどうか
	bool m_isHitPoly;


	//壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* m_pWallPoly[kMaxColHitPolyNum]{};
	//床ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* m_pFloorPoly[kMaxColHitPolyNum]{};
	//壁ポリゴンと判断されたポリゴン数
	int m_wallNum = 0;
	//床ポリゴンと判断されたポリゴン数
	int m_floorNum = 0;
	//PhysicsがCollidableを自由に管理できるように
	friend Physics;
};

