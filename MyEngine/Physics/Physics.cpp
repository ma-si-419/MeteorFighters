#include "Physics.h"
#include <cassert>
#include <cmath>
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

		//当たり判定がカプセルだったら
		if (item->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{
			auto capsuleCol = std::dynamic_pointer_cast<CapsuleColliderData>(item->m_pColData);

			//初期位置を一緒に動かすカプセルであれば
			if (capsuleCol->m_isMoveStartPos)
			{
				//初期位置を一緒に動かす
				MyEngine::Vector3 pos = item->m_nextPos;
				pos.y += capsuleCol->m_length;
				capsuleCol->m_nextStartPos = pos;

			}
		}
	}
	//当たっているものを入れる配列
	std::vector<OnCollideInfo> hitData;
	for (auto& first : m_collidables)
	{
		for (auto& second : m_collidables)
		{
			//当たり判定チェック
			if (IsCheckCollide(first, second))
			{
				//一度入れたものを二度入れないようにチェック
				bool hasFirstColData = false;
				bool hasSecondColData = false;
				for (auto& item : hitData)
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
					hitData.push_back({ first,second });
				}
				if (!hasSecondColData)
				{
					hitData.push_back({ second,first });
				}
			}
		}
	}
	//当たった当たり判定の当たった時の処理を呼ぶ
	for (auto& hitCol : hitData)
	{
		hitCol.OnCollide();
		//物理挙動をするものだけ座標修正を行う
		if (!hitCol.owner->m_pColData->GetIsTrigger() ||
			!hitCol.colider->m_pColData->GetIsTrigger())
		{
			//座標修正
			FixNextPosition(hitCol);
		}
	}
	//座標確定
	ConfirmPosition();
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

	//地面の線の描画
	for (int x = 0; x < 20; x++)
	{
		for (int z = 0; z < 20; z++)
		{
			DrawLine3D(VGet(x * 10.0f - 50.0f,0.0f,-1000.0f),VGet(x * 10.0f - 50.0f,0.0f,1000.0f),GetColor(255,0,0));
			DrawLine3D(VGet(-1000.0f,0.0f, z * 10.0f - 50.0f),VGet(1000.0f,0.0f, z * 10 - 50.0f),GetColor(255,255,0));
		}
	}

}

void Physics::ConfirmPosition()
{
	for (auto& item : m_collidables)
	{
		// Posを更新するので、velocityもそこに移動するvelocityに修正
		MyEngine::Vector3 velocity = item->m_nextPos - item->m_rigidbody.GetPos();
		MyEngine::Vector3 nextPos = item->m_rigidbody.GetPos() + velocity;

		//当たり判定がカプセルだったら
		if (item->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{
			auto capsuleCol = std::dynamic_pointer_cast<CapsuleColliderData>(item->m_pColData);

			//初期位置を一緒に動かすカプセルであれば
			if (capsuleCol->m_isMoveStartPos)
			{
				//初期位置を一緒に動かす
				capsuleCol->m_startPos = capsuleCol->m_nextStartPos;
			}
		}

		// 位置確定
		item->m_rigidbody.SetPos(nextPos);

	}
}

void Physics::FixNextPosition(OnCollideInfo hitCol)
{
	//今回動かす当たり判定
	auto moveCol = hitCol.owner;
	//今回動かない当たり判定
	auto staticCol = hitCol.colider;

	//動かす当たり判定が球で
	if (moveCol->m_pColData->GetKind() == ColliderData::Kind::kSphere)
	{
		//動かない当たり判定が球の場合
		if (staticCol->m_pColData->GetKind() == ColliderData::Kind::kSphere)
		{
			//球のデータにダウンキャスト
			auto staticSphereData = std::dynamic_pointer_cast<SphereColliderData>(staticCol->m_pColData);

			//調整する方向
			MyEngine::Vector3 fixDir = (moveCol->m_nextPos - staticCol->m_nextPos).Normalize();

			//球の半径+ちょっと(半径だけ足すとぶつかっている場所に補正するから)離す
			float fixScale = staticSphereData->m_radius + 0.001f;

			//位置の補正
			moveCol->m_nextPos = (fixDir * fixScale) + staticCol->m_nextPos;
		}
		//動かない当たり判定がカプセルの場合
		else if (staticCol->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{
			//動かない当たり判定をカプセルのデータにダウンキャスト
			auto staticCapsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(staticCol->m_pColData);

			//球とカプセルの当たり判定を取得する
			float distance = Segment_Point_MinLength(staticCol->m_nextPos.CastVECTOR(), staticCapsuleData->m_nextStartPos.CastVECTOR(), moveCol->m_nextPos.CastVECTOR());

			//最近接点を求める

			//カプセルの線
			MyEngine::Vector3 startToEnd = staticCol->m_nextPos - staticCapsuleData->m_nextStartPos;

			//カプセルの線の端から球の座標までのベクトル
			MyEngine::Vector3 startToPoint = moveCol->m_nextPos - staticCapsuleData->m_nextStartPos;

			//線上のどの辺かを求める
			float t = startToEnd.Dot(startToPoint) / startToEnd.SqLength();
			//排他処理
			t = std::fmax(std::fmin(t, 1.0f), 0.0f);
			//最近接点
			MyEngine::Vector3 nearPoint = staticCapsuleData->m_nextStartPos + startToEnd * t;
			//最近接点から動く判定の中心までの単位ベクトル
			MyEngine::Vector3 fixDir = (moveCol->m_nextPos - nearPoint).Normalize();
			//補正の大きさ(カプセルの半径よりも少しだけ大きくする)
			float fixScale = staticCapsuleData->m_radius + 0.001f;
			//位置の補正
			moveCol->m_nextPos = (fixDir * fixScale) + nearPoint;
		}
	}
	//動かす当たり判定がカプセルで
	else if (moveCol->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
	{
		//動かない当たり判定が球の場合
		if (staticCol->m_pColData->GetKind() == ColliderData::Kind::kSphere)
		{
			//動く当たり判定をカプセルのデータにダウンキャスト
			auto moveCapsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(moveCol->m_pColData);

			//動かない当たり判定を球のデータにダウンキャスト
			auto staticSphereData = std::dynamic_pointer_cast<SphereColliderData>(staticCol->m_pColData);

			//球とカプセルの当たり判定を取得する
			float distance = Segment_Point_MinLength(moveCol->m_nextPos.CastVECTOR(), moveCapsuleData->m_nextStartPos.CastVECTOR(), staticCol->m_nextPos.CastVECTOR());

			//最近接点を求める

			//カプセルの線
			MyEngine::Vector3 startToEnd = moveCol->m_nextPos - moveCapsuleData->m_nextStartPos;

			//カプセルの線の端から球の座標までのベクトル
			MyEngine::Vector3 startToPoint = staticCol->m_nextPos - moveCapsuleData->m_nextStartPos;

			//線上のどの辺かを求める
			float t = startToEnd.Dot(startToPoint) / startToEnd.SqLength();
			//排他処理
			t = std::fmax(std::fmin(t, 1.0f), 0.0f);
			//最近接点
			MyEngine::Vector3 nearPoint = moveCapsuleData->m_nextStartPos + startToEnd * t;
			//動かない判定の中心から最近接点の単位ベクトル
			MyEngine::Vector3 fixDir = (nearPoint - staticCol->m_nextPos).Normalize();
			//補正の大きさ
			float fixScale = distance - (moveCapsuleData->m_radius + staticSphereData->m_radius) + 0.001f;
			//位置の補正
			moveCol->m_nextPos = moveCol->m_nextPos + (fixDir * fixScale);
			moveCapsuleData->m_nextStartPos = moveCapsuleData->m_nextStartPos + (fixDir * fixScale);
		}
		//動かない当たり判定がカプセルの場合
		else if (staticCol->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{
			//動く当たり判定をカプセルのデータにダウンキャスト
			auto moveCapsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(moveCol->m_pColData);

			//動かない当たり判定をカプセルのデータにダウンキャスト
			auto staticCapsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(staticCol->m_pColData);

			//カプセルの中心点を取得する
			MyEngine::Vector3 moveCapsuleCenter = moveCol->m_nextPos + (moveCol->m_nextPos - moveCapsuleData->m_nextStartPos) * 0.5f;
			MyEngine::Vector3 staticCapsuleCenter = staticCol->m_nextPos + (staticCol->m_nextPos - staticCapsuleData->m_nextStartPos) * 0.5f;
			
			//動くカプセルの線とと動かないカプセルの中点の最近接点を求める
			MyEngine::Vector3 moveCapsuleStartToEnd = moveCol->m_nextPos - moveCapsuleData->m_nextStartPos;
			MyEngine::Vector3 moveCapsuleStartToPoint = staticCapsuleCenter - moveCapsuleData->m_nextStartPos;
			
			//線上のどの辺に最近接点があるか
			float moveCapsuleRate = moveCapsuleStartToEnd.Dot(moveCapsuleStartToPoint) / moveCapsuleStartToEnd.SqLength();
			//排他処理
			moveCapsuleRate = std::fmax(std::fmin(moveCapsuleRate,1.0f),0.0f);

			//線と中点の最近接点
			MyEngine::Vector3 moveNearestPoint = moveCapsuleData->m_nextStartPos + moveCapsuleStartToEnd * moveCapsuleRate;

			//線と中点の最近接点と動かないカプセルの線の最近接点を求める
			MyEngine::Vector3 staticCapsuleStartToEnd = staticCol->m_nextPos - staticCapsuleData->m_nextStartPos;
			MyEngine::Vector3 staticCapsuleStartToPoint = moveNearestPoint - staticCapsuleData->m_nextStartPos;

			//線上のどの辺に最近接点があるか
			float staticCapsuleRate = staticCapsuleStartToEnd.Dot(staticCapsuleStartToPoint) / staticCapsuleStartToEnd.SqLength();

			//排他処理
			staticCapsuleRate = std::fmax(std::fmin(staticCapsuleRate, 1.0f), 0.0f);

			//最近接点
			MyEngine::Vector3 staticNearestPoint = staticCapsuleData->m_nextStartPos + staticCapsuleStartToEnd * staticCapsuleRate;
			
			

			DrawSphere3D(staticNearestPoint.CastVECTOR(),2,3,GetColor(255,0,0),GetColor(255,0,0),true);

			//補正する方向ベクトル
			MyEngine::Vector3 fixDir = (moveNearestPoint - staticNearestPoint).Normalize();

			//最近接点の距離を取得
			float distance = (moveNearestPoint - staticNearestPoint).Length();

			//補正する大きさ
			float fixScale = moveCapsuleData->m_radius + staticCapsuleData->m_radius + 0.01f - distance;

			moveCol->m_nextPos = moveCol->m_nextPos + (fixDir * fixScale);
			moveCapsuleData->m_nextStartPos = moveCapsuleData->m_nextStartPos + (fixDir * fixScale);

		}
	}
}

bool Physics::IsCheckCollide(std::shared_ptr<Collidable> first, std::shared_ptr<Collidable> second)
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
			float distance = Segment_Segment_MinLength(first->m_nextPos.CastVECTOR(), firstCol->m_nextStartPos.CastVECTOR(),
				second->m_nextPos.CastVECTOR(), secondCol->m_nextStartPos.CastVECTOR());

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
					capsuleData->m_nextStartPos.CastVECTOR(), first->m_nextPos.CastVECTOR());
			}
			else
			{
				capsuleDataBase = first->m_pColData;
				sphereDataBase = second->m_pColData;
				//カプセルの線分のデータを使うためにダウンキャスト
				auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleDataBase);
				//線分と点の最近距離を求める
				distance = Segment_Point_MinLength(first->m_nextPos.CastVECTOR(),
					capsuleData->m_nextStartPos.CastVECTOR(), second->m_nextPos.CastVECTOR());
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
