#include "Physics.h"
#include <cassert>
#include "Vector3.h"
#include "Collidable.h"
#include "CapsuleColliderData.h"
#include "SphereColliderData.h"
Physics::~Physics()
{
}
void Physics::Entry(std::shared_ptr<Collidable> col)
{
	// 登録
	bool found = (std::find(m_collidables.begin(), m_collidables.end(), col) != m_collidables.end());
	if (!found)
	{
		m_collidables.emplace_back(col);
	}
	// 既に登録されてたらエラー
	else
	{
		assert(0 && "指定のcollidableは登録済です。");
	}
}

void Physics::Exit(std::shared_ptr<Collidable> col)
{
	// 登録解除
	bool found = (std::find(m_collidables.begin(), m_collidables.end(), col) != m_collidables.end());
	if (found)
	{
		m_collidables.remove(col);
	}
	// 登録されてなかったらエラー
	else
	{
		assert(0 && "指定のcollidableが登録されていません。");
	}
}

void Physics::Update()
{
	//仮処理
	for (auto& item : m_collidables)
	{
		item->m_nextPos = (item->m_rigidbody.GetPos() + item->m_rigidbody.GetVelo());
	}
	//当たっているものを入れる配列
	std::vector<OnCollideInfo> pushData;
	for (auto& first : m_collidables)
	{
		for (auto& second : m_collidables)
		{
			//当たり判定チェック
			if (CheckCollide(first, second))
			{
				//一度入れたものを二度入れないようにチェック
				bool hasFirstColData = false;
				bool hasSecondColData = false;
				for (auto& item : pushData)
				{
					//すでに入れていたら弾く
					if (item.owner == first)
					{
						hasFirstColData = true;
					}
					if (item.owner == second)
					{
						hasSecondColData = true;
					}
				}
				//弾かれなかった場合当たったものリストに入れる
				if (!hasFirstColData)
				{
					pushData.push_back({ first,second });
					//if (first->GetTag() == ObjectTag::kPlayer)
					//{
					//	printfDx("プレイヤーとぶつかった");
					//}
					//if (first->GetTag() == ObjectTag::kEnemy)
					//{
					//	printfDx("エネミーとぶつかった");
					//}
				}
				if (!hasSecondColData)
				{
					pushData.push_back({ second,first });
					//if (first->GetTag() == ObjectTag::kPlayer)
					//{
					//	printfDx("プレイヤーとぶつかった");
					//}
					//if (first->GetTag() == ObjectTag::kEnemy)
					//{
					//	printfDx("エネミーとぶつかった");
					//}
				}
			}
		}
	}
	//当たった当たり判定の当たった時の処理を呼ぶ
	for (auto& hitCol : pushData)
	{
		hitCol.OnCollide();
	}
	//位置修正
	FixPosition();
}

void Physics::DebugDraw()
{
	//当たり判定の描画を行う
	for (auto& item : m_collidables)
	{
		if (item->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{
			auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(item->m_pColData);

			DrawCapsule3D(capsuleData->m_startPos.CastVECTOR(), item->m_rigidbody.GetPos().CastVECTOR(),
				capsuleData->m_radius, 3, GetColor(50, 50, 255), GetColor(255, 255, 255), false);
		}
		else if (item->m_pColData->GetKind() == ColliderData::Kind::kSphere)
		{
			auto sphereData = std::dynamic_pointer_cast<SphereColliderData>(item->m_pColData);

			DrawSphere3D(item->m_rigidbody.GetPos().CastVECTOR(), sphereData->m_radius, 3, GetColor(255, 255, 0), GetColor(255, 255, 255), false);
		}
	}
}

void Physics::FixPosition()
{
	for (auto& item : m_collidables)
	{
		// Posを更新するので、velocityもそこに移動するvelocityに修正
		MyEngine::Vector3 velocity = item->m_nextPos - item->m_rigidbody.GetPos(); 
		MyEngine::Vector3 nextPos = item->m_rigidbody.GetPos() + velocity;

		// 位置確定
		item->m_rigidbody.SetPos(nextPos);

		//当たり判定がカプセルだったら
		if (item->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{
			auto capsuleCol = std::dynamic_pointer_cast<CapsuleColliderData>(item->m_pColData);
			//伸びるカプセルかどうか取得する
			if (!capsuleCol->m_isMoveStartPos)
			{
				//伸びないカプセルだったら初期位置を一緒に動かす
				capsuleCol->m_startPos = item->m_nextPos;
			}
		}
	}
}

bool Physics::CheckCollide(std::shared_ptr<Collidable> first, std::shared_ptr<Collidable> second)
{
	//第一の当たり判定と第二の当たり判定がおなじものでなければ
	if (first != second)
	{
		//当たり判定の種類を取得
		ColliderData::Kind firstKind = first->m_pColData->GetKind();
		ColliderData::Kind secondKind = second->m_pColData->GetKind();

		//球どうしの当たり判定
		if (firstKind == ColliderData::Kind::kSphere && secondKind == ColliderData::Kind::kSphere)
		{
			//当たり判定を球にダウンキャストする
			auto firstCol = std::dynamic_pointer_cast<SphereColliderData>(first->m_pColData);
			auto secondCol = std::dynamic_pointer_cast<SphereColliderData>(second->m_pColData);

			//当たり判定の距離を求める
			float distance = (first->m_nextPos - second->m_nextPos).Length();

			//球の大きさを合わせたものよりも距離が短ければぶつかっている
			if (distance < firstCol->m_radius + secondCol->m_radius)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		//カプセル同士の当たり判定
		else if (firstKind == ColliderData::Kind::kCapsule && secondKind == ColliderData::Kind::kCapsule)
		{
			//当たり判定をカプセルにダウンキャストする
			auto firstCol = std::dynamic_pointer_cast<CapsuleColliderData>(first->m_pColData);
			auto secondCol = std::dynamic_pointer_cast<CapsuleColliderData>(second->m_pColData);

			//カプセル同士の最短距離
			float distance = Segment_Segment_MinLength(first->m_nextPos.CastVECTOR(), firstCol->m_startPos.CastVECTOR(),
				second->m_nextPos.CastVECTOR(), secondCol->m_startPos.CastVECTOR());

			if (distance < firstCol->m_radius + secondCol->m_radius)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		//球とカプセルの当たり判定
		else if (firstKind == ColliderData::Kind::kSphere && secondKind == ColliderData::Kind::kCapsule ||
			firstKind == ColliderData::Kind::kCapsule && secondKind == ColliderData::Kind::kSphere)
		{
			//球とカプセルのコライダーデータ作成
			std::shared_ptr<ColliderData> sphereDataBase;
			std::shared_ptr<ColliderData> capsuleDataBase;
			float distance;
			//どちらがカプセルなのか球なのか判別してからデータを入れる
			if (firstKind == ColliderData::Kind::kSphere)
			{
				sphereDataBase = first->m_pColData;
				capsuleDataBase = second->m_pColData;

				//カプセルの線分のデータを使うためにダウンキャスト
				auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleDataBase);
				//線分と点の最近距離を求める
				distance = Segment_Point_MinLength(second->m_nextPos.CastVECTOR(),
					capsuleData->m_startPos.CastVECTOR(), first->m_nextPos.CastVECTOR());
			}
			else
			{
				capsuleDataBase = first->m_pColData;
				sphereDataBase = second->m_pColData;
				//カプセルの線分のデータを使うためにダウンキャスト
				auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleDataBase);
				//線分と点の最近距離を求める
				distance = Segment_Point_MinLength(first->m_nextPos.CastVECTOR(),
					capsuleData->m_startPos.CastVECTOR(), second->m_nextPos.CastVECTOR());
			}
			//ダウンキャスト
			auto sphereData = std::dynamic_pointer_cast<SphereColliderData>(sphereDataBase);
			auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleDataBase);

			//円とカプセルの半径よりも円とカプセルの距離が近ければ当たっている
			if (distance < sphereData->m_radius + capsuleData->m_radius)
			{
				return true;
			}
			else
			{
				return false;
			}


		}
		//どれにも当てはまらなかったら
		else
		{
			assert(false);
			return false;
		}
	}
	return false;
}
