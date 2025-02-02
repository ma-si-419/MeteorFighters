#pragma once
#include "Collidable.h"
#include "Character.h"
class Effect;
class EffectManager;
class Attack : public Collidable
{
public:

	/// <summary>
	/// 攻撃のステータス
	/// </summary>
	struct AttackStatus
	{
		MyEngine::Vector3 targetPos;
		int damage = 0;
		float speed = 0;
		int lifeTime = 0;
		float radius = 0;
		std::string attackName = "empty";
		std::string effectName = "empty";
		std::string hitSoundName = "empty";
		Character::AttackKind attackKind = Character::AttackKind::kPhysical;
		Character::AttackHitKind attackHitKind = Character::AttackHitKind::kLow;
	};

public:
	Attack(ObjectTag tag,MyEngine::Vector3 pos);

	virtual ~Attack();

	/// <summary>
	/// 攻撃のステータスの設定
	/// </summary>
	void Init(AttackStatus status,std::shared_ptr<EffectManager> manager);

	/// <summary>
	/// 攻撃の更新(座標の更新など)
	/// </summary>
	void Update();

	/// <summary>
	/// 描画するものがあれば描画する
	/// </summary>
	void Draw();

	/// <summary>
	/// このクラスをけす時に呼び出す
	/// </summary>
	void Final();

	/// <summary>
	/// エフェクトの再生を止める
	/// </summary>
	void StopEffect();

	/// <summary>
	/// エフェクトの再生時間を設定する
	/// </summary>
	/// <param name="time">再生時間</param>
	void SetEffectLifeTime(int time);

	/// <summary>
	/// 攻撃が当たったときのエフェクトの種類を返す
	/// </summary>
	/// <returns>Effeck::EffeckKindをintにキャストしたものを返す</returns>
	int GetHitEffeckKind();

	/// <summary>
	/// 存在しているかを返す
	/// </summary>
	/// <returns>攻撃が存在しているかどうか</returns>
	bool IsExist() { return m_isExist; }

	/// <summary>
	/// 攻撃をけす
	/// </summary>
	void DeleteAttack() { m_isExist = false; }

	/// <summary>
	/// 攻撃のステータスを返す
	/// </summary>
	/// <returns>攻撃のステータス</returns>
	AttackStatus GetStatus() { return m_status; }

	/// <summary>
	/// 攻撃の座標を返す
	/// </summary>
	/// <returns>攻撃の中心座標</returns>
	MyEngine::Vector3 GetPos() { return m_rigidbody.GetPos(); }

	/// <summary>
	/// 1フレーム前の座標を返す
	/// </summary>
	/// <returns>1フレーム前の座標</returns>
	MyEngine::Vector3 GetLastPos() { return m_lastPos; }

	/// <summary>
	/// 自身以外のCollidableとぶつかった時の処理を書く
	/// </summary>
	/// <param name="collider">ぶつかった相手側のコライダー</param>
	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

private:

	/// <summary>
	/// 敵の攻撃とぶつかったときに自身の攻撃を消すかどうか
	/// </summary>
	/// <param name="attack">相手の攻撃</param>
	/// <returns>攻撃を消すならtrue</returns>
	bool IsDelete(std::shared_ptr<Attack> attack);

private:

	//前のフレームの座標
	MyEngine::Vector3 m_lastPos;

	//攻撃のステータス
	AttackStatus m_status;

	//攻撃の移動方向ベクトル
	MyEngine::Vector3 m_dir;

	//シーンに出てから何フレームたったか
	int m_lifeTime;

	//存在しているかどうか
	bool m_isExist;

	//追尾するかどうか
	bool m_isTrack;

	//エフェクト
	std::shared_ptr<Effect> m_pEffect;

	//エフェクトマネージャーを持っておく
	std::shared_ptr<EffectManager> m_pEffectManager;
};