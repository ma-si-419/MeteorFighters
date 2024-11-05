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
	/// ���g�̃^�O��Ԃ�
	/// </summary>
	/// <returns>���g��ObjectTag</returns>
	ObjectTag GetTag() const { return m_tag; }


	/// <summary>
	/// Collidable�ǂ������Ԃ��������̏���������
	/// </summary>
	/// <param name="collider">�Ԃ��������葤��Collidable</param>
	virtual void OnCollide(std::shared_ptr<Collidable> collider) abstract;

protected:
	//�����f�[�^
	Rigidbody m_rigidbody;
	//�����蔻��f�[�^
	std::shared_ptr<ColliderData> m_pColData;
private:
	std::shared_ptr<ColliderData> CreateColliderData(ColliderData::Kind kind);

	ObjectTag m_tag;

	MyEngine::Vector3 m_nextPos;

	//�Փ˂������ɕ������������Ȃ�����true�ɂ���
	bool m_isTrigger;

	//�ړ����Ă邩�ǂ���
	bool m_isMove;

	//�X�e�[�W�̃|���S���ƂԂ��������ǂ���
	bool m_isHitPoly;


	//�ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
	MV1_COLL_RESULT_POLY* m_pWallPoly[kMaxColHitPolyNum]{};
	//���|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
	MV1_COLL_RESULT_POLY* m_pFloorPoly[kMaxColHitPolyNum]{};
	//�ǃ|���S���Ɣ��f���ꂽ�|���S����
	int m_wallNum = 0;
	//���|���S���Ɣ��f���ꂽ�|���S����
	int m_floorNum = 0;
	//Physics��Collidable�����R�ɊǗ��ł���悤��
	friend Physics;
};

