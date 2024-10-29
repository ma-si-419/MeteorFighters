#pragma once
#include "Collidable.h"
#include "CharacterBase.h"
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
		CharacterBase::AttackKind attackKind = CharacterBase::AttackKind::kPhysical;
		CharacterBase::AttackHitKind attackHitKind = CharacterBase::AttackHitKind::kLow;
	};

public:
	Attack(ObjectTag tag,MyEngine::Vector3 pos);

	virtual ~Attack();

	/// <summary>
	/// 攻撃のステータスの設定
	/// </summary>
	void Init(AttackStatus status);

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
	/// 存在しているかを返す
	/// </summary>
	/// <returns>攻撃が存在しているかどうか</returns>
	bool IsExist() { return m_isExist; }

	/// <summary>
	/// 攻撃のステータスを返す
	/// </summary>
	/// <returns>攻撃のステータス</returns>
	AttackStatus GetStatus() { return m_status; }

	/// <summary>
	/// 自身以外のCollidableとぶつかった時の処理を書く
	/// </summary>
	/// <param name="collider">ぶつかった相手側のコライダー</param>
	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
private:

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

};

