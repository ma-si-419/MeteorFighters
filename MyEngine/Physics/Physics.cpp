#include "Physics.h"
#include <cassert>
#include <cmath>
#include "Vector3.h"
#include "Collidable.h"
#include "CapsuleColliderData.h"
#include "SphereColliderData.h"

namespace
{
	//移動したかを判断するための定数
	constexpr float kMoveLange = 0.01f;
	//壁と判断するための定数
	constexpr float kWallPolyYDir = 0.6f;
	//壁と判断する壁の高さ(階段状のもの等を壁と認識しない高さ)
	constexpr float kWallHeight = 5.0f;
	//壁から押し出しをする最大数
	constexpr int kMaxPushWallNum = 16;
	//壁からの押し出しするときの動く大きさ
	constexpr float kColHitSlideLength = 1.0f;

	//移動制限をかける距離
	constexpr float kMoveLimitLength = 710.0f;
}

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
	//更新しないと決められていたら早期リターン
	if (!m_isUpdate) return;

	//仮処理
	for (auto& item : m_collidables)
	{
		item->m_nextPos = (item->m_rigidbody.GetPos() + item->m_rigidbody.GetVelo());

		//当たり判定がカプセルだったら
		if (item->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{
			auto capsuleCol = std::dynamic_pointer_cast<CapsuleColliderData>(item->m_pColData);

			//位置を一緒に動かすカプセルであれば
			if (capsuleCol->m_isMoveEndPos)
			{
				//位置を一緒に動かす
				MyEngine::Vector3 pos = item->m_nextPos;
				pos += capsuleCol->m_lange;
				capsuleCol->m_nextEndPos = pos;

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
					if (item.owner == first && item.colider == second)
					{
						hasFirstColData = true;
					}
					if (item.owner == second && item.colider == first)
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
		//ステージとしか当たり判定を行わないものであればスキップ
		if (hitCol.owner->GetTag() == ObjectTag::kStageObject ||
			hitCol.colider->GetTag() == ObjectTag::kStageObject)
		{
			continue;
		}

		hitCol.OnCollide();
		//物理挙動をするものだけ座標修正を行う
		if (!hitCol.owner->m_pColData->GetIsTrigger() &&
			!hitCol.colider->m_pColData->GetIsTrigger())
		{
			//座標修正
			FixNextPosition(hitCol);
		}
	}
	if (m_stageHandle != -1)
	{
		//ステージの当たり判定
		StageColUpdate();
	}

	//座標確定
	ConfirmPosition();
}

void Physics::StageColUpdate()
{
	//当たり判定リスト
	for (auto& item : m_collidables)
	{
		//物理挙動をしないものは判定しない
		if (item->m_isTrigger)return;

		MV1_COLL_RESULT_POLY_DIM hitDim;

		if (item->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{

			auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(item->m_pColData);

			//周囲にあるステージポリゴンを取得する
			//移動距離も含めて検出範囲を計算する
			hitDim = MV1CollCheck_Capsule(m_stageHandle, -1, item->m_rigidbody.GetPos().CastVECTOR(), colData->m_nextEndPos.CastVECTOR(), colData->m_radius + (item->m_nextPos - item->m_rigidbody.GetPos()).Length());
		}
		else if (item->m_pColData->GetKind() == ColliderData::Kind::kSphere)
		{
			auto colData = std::dynamic_pointer_cast<SphereColliderData>(item->m_pColData);

			//周囲にあるステージポリゴンを取得する
			//移動距離も含めて検出範囲を計算する
			hitDim = MV1CollCheck_Sphere(m_stageHandle, -1, item->m_rigidbody.GetPos().CastVECTOR(), colData->m_radius + (item->m_nextPos - item->m_rigidbody.GetPos()).Length());
		}

		m_hitDim = hitDim;

		//移動していたら移動フラグを立てる
		if ((item->m_nextPos - item->m_rigidbody.GetPos()).Length() > kMoveLange)
		{
			item->m_isMove = true;
		}
		else
		{
			item->m_isMove = false;
		}

		//壁と床の当たり判定を行う
		CheckWallAndFloor(item);
		//壁との当たり判定処理
		FixPositionWithWall(item);
		//床との当たり判定処理
		FixNowPositionWithFloor(item);
	}
}

void Physics::SetStage(int stageHandle)
{
	m_stageHandle = stageHandle;
}

void Physics::DebugDraw()
{
	//当たり判定の描画を行う
	for (auto& item : m_collidables)
	{
		if (item->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{
			auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(item->m_pColData);

			DrawCapsule3D(capsuleData->m_endPos.CastVECTOR(), item->m_rigidbody.GetPos().CastVECTOR(),
				capsuleData->m_radius, 3, GetColor(50, 50, 255), GetColor(255, 255, 255), false);
		}
		else if (item->m_pColData->GetKind() == ColliderData::Kind::kSphere)
		{
			auto sphereData = std::dynamic_pointer_cast<SphereColliderData>(item->m_pColData);

			DrawSphere3D(item->m_rigidbody.GetPos().CastVECTOR(), sphereData->m_radius, 3, GetColor(255, 255, 0), GetColor(255, 255, 255), false);
		}
	}

	////地面の線の描画
	//for (int x = 0; x < 20; x++)
	//{
	//	for (int z = 0; z < 20; z++)
	//	{
	//		DrawLine3D(VGet(x * 10.0f - 50.0f, 0.0f, -1000.0f), VGet(x * 10.0f - 50.0f, 0.0f, 1000.0f), GetColor(255, 0, 0));
	//		DrawLine3D(VGet(-1000.0f, 0.0f, z * 10.0f - 50.0f), VGet(1000.0f, 0.0f, z * 10 - 50.0f), GetColor(255, 255, 0));
	//	}
	//}

}

bool Physics::GetHitObject(MyEngine::Vector3 pos, std::shared_ptr<ColliderData> collide, ObjectTag tag)
{

	//当たり判定をとるものリスト
	std::list<std::shared_ptr<Collidable>> checkColList;

	//登録している当たり判定をすべて
	for (auto col : m_collidables)
	{
		//当たり判定をとりたいタグと同じであれば
		if (col->GetTag() == tag)
		{
			//当たり判定をとるようにする
			checkColList.push_back(col);
		}
	}

	//当たり判定をとりたいものがカプセルであれば
	if (collide->GetKind() == ColliderData::Kind::kCapsule)
	{

	}
	else if (collide->GetKind() == ColliderData::Kind::kSphere)
	{

	}

	return false;
}

void Physics::ConfirmPosition()
{
	for (auto& item : m_collidables)
	{
		// Posを更新するので、velocityもそこに移動するvelocityに修正
		MyEngine::Vector3 velocity = item->m_nextPos - item->m_rigidbody.GetPos();
		MyEngine::Vector3 nextPos = item->m_rigidbody.GetPos() + velocity;

		MyEngine::Vector3 centerPos = MyEngine::Vector3(0, 0, 0);

		//移動制限をつける

		//IsTriggerでないものは移動制限をかける
		if (!item->m_pColData->GetIsTrigger())
		{
			if ((nextPos - centerPos).Length() > kMoveLimitLength)
			{
				//座標を補正
				nextPos = (nextPos - centerPos).Normalize() * kMoveLimitLength;
			}
			//一定よりも下に行かないようにする
			if (nextPos.y < 0.0f)
			{
				nextPos.y = 0.0f;
			}
		}

		//当たり判定がカプセルだったら
		if (item->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{
			auto capsuleCol = std::dynamic_pointer_cast<CapsuleColliderData>(item->m_pColData);

			//初期位置を一緒に動かすカプセルであれば
			if (capsuleCol->m_isMoveEndPos)
			{
				//初期位置を一緒に動かす
				capsuleCol->m_endPos = capsuleCol->m_nextEndPos;
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
			auto moveSphereData = std::dynamic_pointer_cast<SphereColliderData>(moveCol->m_pColData);
			auto staticSphereData = std::dynamic_pointer_cast<SphereColliderData>(staticCol->m_pColData);

			//調整する方向
			MyEngine::Vector3 fixDir = (moveCol->m_nextPos - staticCol->m_nextPos).Normalize();

			//球の半径+ちょっと(半径だけ足すとぶつかっている場所に補正するから)離す
			float fixScale = staticSphereData->m_radius + moveSphereData->m_radius + 0.001f;

			//位置の補正
			moveCol->m_nextPos = (fixDir * fixScale) + staticCol->m_nextPos;
		}
		//動かない当たり判定がカプセルの場合
		else if (staticCol->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{
			//動かない当たり判定をカプセルのデータにダウンキャスト
			auto staticCapsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(staticCol->m_pColData);

			//球とカプセルの当たり判定を取得する
			float distance = Segment_Point_MinLength(staticCol->m_nextPos.CastVECTOR(), staticCapsuleData->m_nextEndPos.CastVECTOR(), moveCol->m_nextPos.CastVECTOR());

			//最近接点を求める

			//カプセルの線
			MyEngine::Vector3 startToEnd = staticCol->m_nextPos - staticCapsuleData->m_nextEndPos;

			//カプセルの線の端から球の座標までのベクトル
			MyEngine::Vector3 startToPoint = moveCol->m_nextPos - staticCapsuleData->m_nextEndPos;

			//線上のどの辺かを求める
			float t = startToEnd.Dot(startToPoint) / startToEnd.SqLength();
			//排他処理
			t = std::fmax(std::fmin(t, 1.0f), 0.0f);
			//最近接点
			MyEngine::Vector3 nearPoint = staticCapsuleData->m_nextEndPos + startToEnd * t;
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
			float distance = Segment_Point_MinLength(moveCol->m_nextPos.CastVECTOR(), moveCapsuleData->m_nextEndPos.CastVECTOR(), staticCol->m_nextPos.CastVECTOR());

			//最近接点を求める

			//カプセルの線
			MyEngine::Vector3 startToEnd = moveCol->m_nextPos - moveCapsuleData->m_nextEndPos;

			//カプセルの線の端から球の座標までのベクトル
			MyEngine::Vector3 startToPoint = staticCol->m_nextPos - moveCapsuleData->m_nextEndPos;

			//線上のどの辺かを求める
			float t = startToEnd.Dot(startToPoint) / startToEnd.SqLength();
			//排他処理
			t = std::fmax(std::fmin(t, 1.0f), 0.0f);
			//最近接点
			MyEngine::Vector3 nearPoint = moveCapsuleData->m_nextEndPos + startToEnd * t;
			//動かない判定の中心から最近接点の単位ベクトル
			MyEngine::Vector3 fixDir = (nearPoint - staticCol->m_nextPos).Normalize();
			//補正の大きさ
			float fixScale = distance - (moveCapsuleData->m_radius + staticSphereData->m_radius) + 0.001f;
			//位置の補正
			moveCol->m_nextPos = moveCol->m_nextPos + (fixDir * fixScale);
			moveCapsuleData->m_nextEndPos = moveCapsuleData->m_nextEndPos + (fixDir * fixScale);
		}
		//動かない当たり判定がカプセルの場合
		else if (staticCol->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{
			//動く当たり判定をカプセルのデータにダウンキャスト
			auto moveCapsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(moveCol->m_pColData);

			//動かない当たり判定をカプセルのデータにダウンキャスト
			auto staticCapsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(staticCol->m_pColData);

			//カプセルの中心点を取得する
			MyEngine::Vector3 moveCapsuleCenter = moveCol->m_nextPos + (moveCol->m_nextPos - moveCapsuleData->m_nextEndPos) * 0.5f;
			MyEngine::Vector3 staticCapsuleCenter = staticCol->m_nextPos + (staticCol->m_nextPos - staticCapsuleData->m_nextEndPos) * 0.5f;

			//動くカプセルの線とと動かないカプセルの中点の最近接点を求める
			MyEngine::Vector3 moveCapsuleStartToEnd = moveCol->m_nextPos - moveCapsuleData->m_nextEndPos;
			MyEngine::Vector3 moveCapsuleStartToPoint = staticCapsuleCenter - moveCapsuleData->m_nextEndPos;

			//線上のどの辺に最近接点があるか
			float moveCapsuleRate = moveCapsuleStartToEnd.Dot(moveCapsuleStartToPoint) / moveCapsuleStartToEnd.SqLength();
			//排他処理
			moveCapsuleRate = std::fmax(std::fmin(moveCapsuleRate, 1.0f), 0.0f);

			//線と中点の最近接点
			MyEngine::Vector3 moveNearestPoint = moveCapsuleData->m_nextEndPos + moveCapsuleStartToEnd * moveCapsuleRate;

			//線と中点の最近接点と動かないカプセルの線の最近接点を求める
			MyEngine::Vector3 staticCapsuleStartToEnd = staticCol->m_nextPos - staticCapsuleData->m_nextEndPos;
			MyEngine::Vector3 staticCapsuleStartToPoint = moveNearestPoint - staticCapsuleData->m_nextEndPos;

			//線上のどの辺に最近接点があるか
			float staticCapsuleRate = staticCapsuleStartToEnd.Dot(staticCapsuleStartToPoint) / staticCapsuleStartToEnd.SqLength();

			//排他処理
			staticCapsuleRate = std::fmax(std::fmin(staticCapsuleRate, 1.0f), 0.0f);

			//最近接点
			MyEngine::Vector3 staticNearestPoint = staticCapsuleData->m_nextEndPos + staticCapsuleStartToEnd * staticCapsuleRate;



			DrawSphere3D(staticNearestPoint.CastVECTOR(), 2, 3, GetColor(255, 0, 0), GetColor(255, 0, 0), true);

			//補正する方向ベクトル
			MyEngine::Vector3 fixDir = (moveNearestPoint - staticNearestPoint).Normalize();

			//最近接点の距離を取得
			float distance = (moveNearestPoint - staticNearestPoint).Length();

			//補正する大きさ
			float fixScale = moveCapsuleData->m_radius + staticCapsuleData->m_radius + 0.01f - distance;

			moveCol->m_nextPos = moveCol->m_nextPos + (fixDir * fixScale);
			moveCapsuleData->m_nextEndPos = moveCapsuleData->m_nextEndPos + (fixDir * fixScale);

		}
	}
}

void Physics::CheckWallAndFloor(std::shared_ptr<Collidable> collider)
{
	//壁ポリゴンと床ポリゴンの数を初期化する
	collider->m_wallNum = 0;
	collider->m_floorNum = 0;

	//検出されたポリゴンの数だけ繰り返す
	for (int i = 0; i < m_hitDim.HitNum; i++)
	{
		//ポリゴンの法線のY成分が壁ポリゴンと判断する値に達しているかどうかで壁か床か判断する
		if (m_hitDim.Dim[i].Normal.y < kWallPolyYDir && m_hitDim.Dim[i].Normal.y > -kWallPolyYDir)
		{
			// 壁ポリゴンと判断された場合でも、当たり判定のＹ座標より一定値高いポリゴンのみ当たり判定を行う
			// 階段状のものなどに引っかからないようにするためのif文
			if (m_hitDim.Dim[i].Position[0].y > collider->m_rigidbody.GetPos().y + kWallHeight ||
				m_hitDim.Dim[i].Position[1].y > collider->m_rigidbody.GetPos().y + kWallHeight ||
				m_hitDim.Dim[i].Position[2].y > collider->m_rigidbody.GetPos().y + kWallHeight)
			{
				//ポリゴンの数が限界数に達していなければポリゴンを配列に追加
				if (collider->m_wallNum < kMaxColHitPolyNum)
				{
					//ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
					collider->m_pWallPoly[collider->m_wallNum] = &m_hitDim.Dim[i];

					//壁ポリゴンの数を加算する
					collider->m_wallNum++;
				}
			}
		}
		else
		{
			//ポリゴンの数が限界数に達していなければポリゴンを配列に追加
			if (collider->m_floorNum < kMaxColHitPolyNum)
			{
				//ポリゴンの構造体のアドレスを床ポリゴンの配列に追加する
				collider->m_pFloorPoly[collider->m_floorNum] = &m_hitDim.Dim[i];

				//床ポリゴンの数を加算する
				collider->m_floorNum++;
			}
		}
	}
}

void Physics::FixPositionWithWall(std::shared_ptr<Collidable> collider)
{
	//壁ポリゴンがない場合は何もしない
	if (collider->m_wallNum == 0)
	{
		//壁に当たっていないフラグを立てる
		collider->m_pColData->SetIsWall(false);

		return;
	}
	//壁ポリゴンとの当たり判定
	//ステージに当たったかどうかフラグをfalseにしておく
	collider->m_isHitPoly = false;

	auto colKind = collider->m_pColData->GetKind();


	//移動したかどうかで処理を分岐させる
	if (collider->m_isMove)
	{
		//当たり判定の種類によって処理を分岐させる
		if (colKind == ColliderData::Kind::kCapsule)
		{
			auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(collider->m_pColData);

			//壁ポリゴンの数だけ繰り返し
			for (int i = 0; i < collider->m_wallNum; i++)
			{
				//i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
				m_pPoly = collider->m_pWallPoly[i];
				//ポリゴンと当たり判定が当たっていなければ次のカウントへ
				if (!HitCheck_Capsule_Triangle(colData->m_nextEndPos.CastVECTOR(), collider->m_nextPos.CastVECTOR(), colData->m_radius,
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))continue;

				//ここに来たら当たっているということなので当たっているフラグを立てる
				collider->m_isHitPoly = true;

				//壁を考慮した移動を外積を使って計算
				MyEngine::Vector3 slideVec;

				//進行方向ベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを計算
				slideVec = (collider->m_nextPos - collider->m_rigidbody.GetPos()).Cross(m_pPoly->Normal);

				//計算したベクトルと壁ポリゴンの法線ベクトルに垂直なベクトル
				//元の移動成分から壁方向の移動成分を抜いたベクトル
				slideVec = MyEngine::Vector3(m_pPoly->Normal).Cross(slideVec);

				//それを移動前の座標に足したものを新たな座標にする
				collider->m_nextPos = collider->m_nextPos + slideVec;

			}
		}
		else if (colKind == ColliderData::Kind::kSphere)
		{
			auto colData = std::dynamic_pointer_cast<SphereColliderData>(collider->m_pColData);
			//壁ポリゴンの数だけ繰り返し
			for (int i = 0; i < collider->m_wallNum; i++)
			{
				//i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
				m_pPoly = collider->m_pWallPoly[i];
				//ポリゴンと当たり判定が当たっていなければ次のカウントへ
				if (!HitCheck_Sphere_Triangle(collider->m_nextPos.CastVECTOR(), colData->m_radius,
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))continue;

				//ここに来たら当たっているということなので当たっているフラグを立てる
				collider->m_isHitPoly = true;

				//壁を考慮した移動を外積を使って計算
				MyEngine::Vector3 slideVec;

				//進行方向ベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを計算
				slideVec = (collider->m_nextPos - collider->m_rigidbody.GetPos()).Cross(m_pPoly->Normal);

				//計算したベクトルと壁ポリゴンの法線ベクトルに垂直なベクトル
				//元の移動成分から壁方向の移動成分を抜いたベクトル
				slideVec = MyEngine::Vector3(m_pPoly->Normal).Cross(slideVec);

				//それを移動前の座標に足したものを新たな座標にする
				collider->m_nextPos = collider->m_nextPos + slideVec;
			}
		}
	}
	else
	{
		//移動していない場合の処理

		//壁ポリゴンの数だけ繰り返し
		for (int i = 0; i < collider->m_wallNum; i++)
		{
			//i番目の壁ポリゴンのアドレスを壁ポリゴン配列から取得
			m_pPoly = collider->m_pWallPoly[i];

			//ポリゴンに当たっていたら当たったフラグを立てたうえでループから抜ける

			//当たり判定の種類によって処理を分岐させる
			if (colKind == ColliderData::Kind::kCapsule)
			{
				auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(collider->m_pColData);

				if (HitCheck_Capsule_Triangle(colData->m_nextEndPos.CastVECTOR(), collider->m_nextPos.CastVECTOR(), colData->m_radius,
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
				{
					collider->m_isHitPoly = true;
				}

			}
			else if (colKind == ColliderData::Kind::kSphere)
			{
				auto colData = std::dynamic_pointer_cast<SphereColliderData>(collider->m_pColData);

				if (HitCheck_Sphere_Triangle(collider->m_nextPos.CastVECTOR(), colData->m_radius,
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
				{
					collider->m_isHitPoly = true;
				}
			}
		}
	}

	//壁に当たっていたら壁から押し出す処理を行う
	if (collider->m_isHitPoly)
	{
		FixPositionWithWallInternal(collider);

		collider->m_pColData->SetIsWall(true);
	}
}

void Physics::FixPositionWithWallInternal(std::shared_ptr<Collidable> collider)
{
	//壁から押し出し処理をする最大数だけ繰り返し
	for (int i = 0; i < kMaxPushWallNum; i++)
	{
		//当たっている可能性のある壁ポリゴンをすべて見る
		bool isHitWall = false;

		//当たり判定の種類
		auto colKind = collider->m_pColData->GetKind();


		//当たり判定の種類によって処理を分岐させる
		if (colKind == ColliderData::Kind::kCapsule)
		{
			auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(collider->m_pColData);

			//壁ポリゴンの数だけ繰り返し
			for (int j = 0; j < collider->m_wallNum; j++)
			{
				//j番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
				m_pPoly = collider->m_pWallPoly[j];


				//当たり判定と当たっているかを判定
				if (!HitCheck_Capsule_Triangle(collider->m_nextPos.CastVECTOR(), (collider->m_nextPos + colData->m_lange).CastVECTOR(), colData->m_radius,
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))continue;

				//当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
				collider->m_nextPos = collider->m_nextPos + (MyEngine::Vector3(m_pPoly->Normal) * kColHitSlideLength);

				//移動したうえで壁ポリゴンと接触しているかどうかを判定
				for (int k = 0; k < collider->m_wallNum; k++)
				{
					//当たっていたらループを抜ける
					m_pPoly = collider->m_pWallPoly[k];
					if (HitCheck_Capsule_Triangle(collider->m_nextPos.CastVECTOR(), (collider->m_nextPos + colData->m_lange).CastVECTOR(), colData->m_radius,
						m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
					{
						collider->m_isHitPoly = true;
						break;
					}
				}
			}
		}
		else if (colKind == ColliderData::Kind::kSphere)
		{
			auto colData = std::dynamic_pointer_cast<SphereColliderData>(collider->m_pColData);
			//壁ポリゴンの数だけ繰り返し
			for (int j = 0; j < collider->m_wallNum; j++)
			{
				//j番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
				m_pPoly = collider->m_pWallPoly[j];


				//当たり判定と当たっているかを判定
				if (!HitCheck_Sphere_Triangle(collider->m_nextPos.CastVECTOR(), colData->m_radius,
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))continue;

				//当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
				collider->m_nextPos = collider->m_nextPos + (MyEngine::Vector3(m_pPoly->Normal) * kColHitSlideLength);

				//移動したうえで壁ポリゴンと接触しているかどうかを判定
				for (int k = 0; k < collider->m_wallNum; k++)
				{
					//当たっていたらループを抜ける
					m_pPoly = collider->m_pWallPoly[k];
					if (HitCheck_Sphere_Triangle(collider->m_nextPos.CastVECTOR(), colData->m_radius,
						m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
					{
						collider->m_isHitPoly = true;
						break;
					}
				}
			}

			//すべてのポリゴンと当たっていなければループ終了
			if (!collider->m_isHitPoly)break;
		}
		//ループ終了
		if (!collider->m_isHitPoly)break;
	}
}

void Physics::FixNowPositionWithFloor(std::shared_ptr<Collidable> collider)
{
	//床ポリゴンがない場合
	if (collider->m_floorNum == 0)
	{
		//床とぶつかっているかどうかをfalseにしておく
		collider->m_pColData->SetIsGround(false);
		return;
	}

	//床ポリゴンとの当たり判定処理

	//当たったかどうかのフラグ初期化
	bool isHitFlag = false;



	//物体が上昇していたら
	if ((collider->m_nextPos.y - collider->m_rigidbody.GetPos().y) > 0)
	{
		//天井にぶつかる処理を行う
		//一番低い天井にぶつけるために初期化
		float polyMinPosY = 0.0f;

		auto colKind = collider->m_pColData->GetKind();

		if (colKind == ColliderData::Kind::kCapsule)
		{

			auto capsule = std::dynamic_pointer_cast<CapsuleColliderData>(collider->m_pColData);

			//床のポリゴンの数だけ繰り返し
			for (int i = 0; i < collider->m_floorNum; i++)
			{
				//i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
				m_pPoly = collider->m_pFloorPoly[i];

				//始点から終点までの間
				m_lineRes = HitCheck_Line_Triangle(collider->m_nextPos.CastVECTOR(), (collider->m_nextPos + capsule->m_lange).CastVECTOR(),
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]);

				//接触していなければ何もしない
				if (!m_lineRes.HitFlag)continue;

				//天井ポリゴンが今まで検出されたポリゴンより低い場合処理を通す
				if (polyMinPosY < m_lineRes.Position.y)
				{
					//ポリゴンに当たったフラグを立てる
					isHitFlag = true;

					//接触したY座標保存する
					polyMinPosY = m_lineRes.Position.y;
				}

			}
		}
		else if (colKind == ColliderData::Kind::kSphere)
		{
			auto sphere = std::dynamic_pointer_cast<SphereColliderData>(collider->m_pColData);

			//床のポリゴンの数だけ繰り返し
			for (int i = 0; i < collider->m_floorNum; i++)
			{
				//i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
				m_pPoly = collider->m_pFloorPoly[i];

				VECTOR nextPos = collider->m_nextPos.CastVECTOR();

				//点との当たり判定
				m_lineRes = HitCheck_Line_Triangle(collider->m_nextPos.CastVECTOR(), (collider->m_nextPos + MyEngine::Vector3(0.1f, 0.0f, 0.0f)).CastVECTOR(),
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]);

				//当たっていなければ処理をしない
				if (!m_lineRes.HitFlag)continue;

				//天井ポリゴンが今まで検出されたポリゴンより低い場合処理を通す
				if (polyMinPosY < m_lineRes.Position.y)
				{
					//ポリゴンに当たったフラグを立てる
					isHitFlag = true;

					//接触したY座標保存する
					polyMinPosY = m_lineRes.Position.y;
				}

			}
		}

		//接触したポリゴンがあれば
		if (isHitFlag)
		{
			if (colKind == ColliderData::Kind::kCapsule)
			{
				auto capsule = std::dynamic_pointer_cast<CapsuleColliderData>(collider->m_pColData);

				//接触した場合はプレイヤーのY座標を接触座標をもとに更新
				collider->m_nextPos.y = polyMinPosY - capsule->m_radius;
			}
			else if (colKind == ColliderData::Kind::kSphere)
			{
				auto sphere = std::dynamic_pointer_cast<SphereColliderData>(collider->m_pColData);

				//接触した場合はプレイヤーのY座標を接触座標をもとに更新
				collider->m_nextPos.y = polyMinPosY - sphere->m_radius;
			}
		}
	}
	//床ポリゴンとの当たり判定
	else
	{
		//一番高い床ポリゴンにぶつけるための判定用変数を初期化
		float polyMaxPosY = 0.0f;

		//床ポリゴンに当たったかどうかのフラグをfalseにしておく
		collider->m_isHitPoly = false;
		auto colKind = collider->m_pColData->GetKind();
		if (colKind == ColliderData::Kind::kCapsule)
		{

			auto capsule = std::dynamic_pointer_cast<CapsuleColliderData>(collider->m_pColData);

			//床のポリゴンの数だけ繰り返し
			for (int i = 0; i < collider->m_floorNum; i++)
			{
				//i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
				m_pPoly = collider->m_pFloorPoly[i];

				//始点から終点までの間
				m_lineRes = HitCheck_Line_Triangle((collider->m_nextPos + capsule->m_lange).CastVECTOR(), collider->m_nextPos.CastVECTOR(),
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]);

				//接触していなければ何もしない
				if (!m_lineRes.HitFlag)continue;

				//すでに当たったポリゴンがあり、かつ今まで検出した床ポリゴンより低い場合は何もしない
				if (collider->m_isHitPoly && polyMaxPosY > m_lineRes.Position.y) continue;

				//ポリゴンに当たったフラグを立てる
				collider->m_isHitPoly = true;

				//接触したY座標保存する
				polyMaxPosY = m_lineRes.Position.y;

			}
		}
		else if (colKind == ColliderData::Kind::kSphere)
		{
			auto sphere = std::dynamic_pointer_cast<SphereColliderData>(collider->m_pColData);

			//床のポリゴンの数だけ繰り返し
			for (int i = 0; i < collider->m_floorNum; i++)
			{
				//i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
				m_pPoly = collider->m_pFloorPoly[i];

				VECTOR nextPos = collider->m_nextPos.CastVECTOR();

				//点との当たり判定
				m_lineRes = HitCheck_Line_Triangle((collider->m_nextPos + MyEngine::Vector3(0.0f, sphere->m_radius, 0.0f)).CastVECTOR(), (collider->m_nextPos + MyEngine::Vector3(0.0f, -sphere->m_radius, 0.0f)).CastVECTOR(),
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]);

				//接触していなければ何もしない
				if (!m_lineRes.HitFlag)continue;

				//すでに当たったポリゴンがあり、かつ今まで検出した床ポリゴンより低い場合は何もしない
				if (collider->m_isHitPoly && polyMaxPosY > m_lineRes.Position.y) continue;

				//ポリゴンに当たったフラグを立てる
				collider->m_isHitPoly = true;

				//接触したY座標保存する
				polyMaxPosY = m_lineRes.Position.y;

			}
		}


		//接触したポリゴンがあれば
		if (collider->m_isHitPoly)
		{
			if (collider->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
			{
				auto capsule = std::dynamic_pointer_cast<CapsuleColliderData>(collider->m_pColData);

				//接触したポリゴンで一番高いY座標を当たり判定のY座標にする
				collider->m_nextPos.y = polyMaxPosY + capsule->m_radius;

				capsule->m_nextEndPos.y = polyMaxPosY + capsule->m_radius + capsule->m_lange.y;
			}
			else if (collider->m_pColData->GetKind() == ColliderData::Kind::kSphere)
			{
				auto sphere = std::dynamic_pointer_cast<SphereColliderData>(collider->m_pColData);

				collider->m_nextPos.y = polyMaxPosY + sphere->m_radius;
			}

			//床に当たっているフラグを立てる
			collider->m_pColData->SetIsGround(true);
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
			float distance = Segment_Segment_MinLength(first->m_nextPos.CastVECTOR(), firstCol->m_nextEndPos.CastVECTOR(),
				second->m_nextPos.CastVECTOR(), secondCol->m_nextEndPos.CastVECTOR());

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
					capsuleData->m_nextEndPos.CastVECTOR(), first->m_nextPos.CastVECTOR());
			}
			else
			{
				capsuleDataBase = first->m_pColData;
				sphereDataBase = second->m_pColData;
				//カプセルの線分のデータを使うためにダウンキャスト
				auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleDataBase);
				//線分と点の最近距離を求める
				distance = Segment_Point_MinLength(first->m_nextPos.CastVECTOR(),
					capsuleData->m_nextEndPos.CastVECTOR(), second->m_nextPos.CastVECTOR());
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
