#include "Rock.h"
#include "DxLib.h"
#include "SphereColliderData.h"

namespace
{
	//飛んでいくときの初速の最大速度
	constexpr float kMaxSpeed = 5.0f;
	//飛んでいくときの初速の最小速度
	constexpr float kMinSpeed = 2.0f;

	//飛んでいくときの初速の最大角度
	constexpr int kMaxAngle = 120;
	//飛んでいくときの初速の最小角度
	constexpr int kMinAngle = 60;
	//度数法から弧度法へ変換する値
	constexpr float kToRadian = DX_PI_F / 180;

	//消えていくまでの時間
	constexpr int kAlphaSubStartTime = 90;

	//消えていく速さ
	constexpr int kAlphaSubSpeed = 15;

	//当たり判定のサイズ
	constexpr float kColliderSize = 1.0f;

	//重力をかけ始める時間
	constexpr int kGravityStartTime = 30;

	//重力の大きさ
	constexpr float kGravity = 0.1f;

	//減速率
	constexpr float kDeceleration = 0.9f;

}

Rock::Rock() :
	ObjectBase(ColliderData::Kind::kSphere),
	m_moveVec(0.0f, 0.0f, 0.0f),
	m_time(0)
{
}

Rock::~Rock()
{

}

void Rock::Init()
{
	//当たり判定のサイズを初期化
	auto sphere = std::dynamic_pointer_cast<SphereColliderData>(m_pColData);
	sphere->m_radius = kColliderSize;
}

void Rock::Update()
{
	//消えていく処理
	m_time++;
	//一定時間経ったら
	if (m_time > kAlphaSubStartTime)
	{
		//アルファ値を減らす
		m_alpha -= kAlphaSubSpeed;
		//アルファ値が0以下になったら
		if (m_alpha <= 0)
		{
			//消す
			m_alpha = 0;
			m_isExist = false;
		}
	}

	//減速
	m_moveVec *= kDeceleration;

	//重力をかける
	if (m_time > kGravityStartTime)
	{
		m_moveVec.y -= kGravity;
	}

	//移動
	m_rigidbody.SetVelo(m_moveVec);
}

void Rock::SetMoveVec(MyEngine::Vector3 bumpPos, MyEngine::Vector3 characterPos)
{
	//ぶつかった場所からキャラクターの座標までのベクトルを求める
	m_moveVec = characterPos - bumpPos;
	//正規化
	m_moveVec = m_moveVec.Normalize();
	//ランダムでベクトルの角度を変える
	MATRIX randomX = MGetRotX(static_cast<float>((GetRand(kMaxAngle - kMinAngle) + kMinAngle) * kToRadian));
	MATRIX randomY = MGetRotY(static_cast<float>((GetRand(kMaxAngle - kMinAngle) + kMinAngle) * kToRadian));
	MATRIX mat = MMult(randomX, randomY);
	//ベクトルを変換
	m_moveVec = m_moveVec.MatTransform(mat);

	//速度をランダムで変える
	float speed = GetRand(kMaxSpeed - kMinSpeed) + kMinSpeed;
	m_moveVec *= speed;

	//初期位置をセット
	m_rigidbody.SetPos(bumpPos);
}
