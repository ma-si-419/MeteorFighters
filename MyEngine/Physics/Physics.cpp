#include "Physics.h"
#include <cassert>
#include <cmath>
#include "Vector3.h"
#include "Collidable.h"
#include "CapsuleColliderData.h"
#include "SphereColliderData.h"

namespace
{
	//�ړ��������𔻒f���邽�߂̒萔
	constexpr float kMoveLange = 0.01f;
	//�ǂƔ��f���邽�߂̒萔
	constexpr float kWallPolyYDir = 0.6f;
	//�ǂƔ��f����ǂ̍���(�K�i��̂��̓���ǂƔF�����Ȃ�����)
	constexpr float kWallHeight = 5.0f;
	//�ǂ��牟���o��������ő吔
	constexpr int kMaxPushWallNum = 16;
	//�ǂ���̉����o������Ƃ��̓����傫��
	constexpr float kColHitSlideLength = 1.0f;

	//�ړ������������鋗��
	constexpr float kMoveLimitLength = 710.0f;
}

Physics::~Physics()
{
}
void Physics::Entry(std::shared_ptr<Collidable> col)
{
	// �o�^
	bool found = (std::find(m_collidables.begin(), m_collidables.end(), col) != m_collidables.end());
	if (!found)
	{
		m_collidables.emplace_back(col);
	}
	// ���ɓo�^����Ă���G���[
	else
	{
		assert(0 && "�w���collidable�͓o�^�ςł��B");
	}
}

void Physics::Exit(std::shared_ptr<Collidable> col)
{
	// �o�^����
	bool found = (std::find(m_collidables.begin(), m_collidables.end(), col) != m_collidables.end());
	if (found)
	{
		m_collidables.remove(col);
	}
	// �o�^����ĂȂ�������G���[
	else
	{
		assert(0 && "�w���collidable���o�^����Ă��܂���B");
	}
}

void Physics::Update()
{
	//�X�V���Ȃ��ƌ��߂��Ă����瑁�����^�[��
	if (!m_isUpdate) return;

	//������
	for (auto& item : m_collidables)
	{
		item->m_nextPos = (item->m_rigidbody.GetPos() + item->m_rigidbody.GetVelo());

		//�����蔻�肪�J�v�Z����������
		if (item->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{
			auto capsuleCol = std::dynamic_pointer_cast<CapsuleColliderData>(item->m_pColData);

			//�ʒu���ꏏ�ɓ������J�v�Z���ł����
			if (capsuleCol->m_isMoveEndPos)
			{
				//�ʒu���ꏏ�ɓ�����
				MyEngine::Vector3 pos = item->m_nextPos;
				pos += capsuleCol->m_lange;
				capsuleCol->m_nextEndPos = pos;

			}
		}
	}
	//�������Ă�����̂�����z��
	std::vector<OnCollideInfo> hitData;
	for (auto& first : m_collidables)
	{
		for (auto& second : m_collidables)
		{
			//�����蔻��`�F�b�N
			if (IsCheckCollide(first, second))
			{
				//��x���ꂽ���̂��x����Ȃ��悤�Ƀ`�F�b�N
				bool hasFirstColData = false;
				bool hasSecondColData = false;
				for (auto& item : hitData)
				{
					//���łɓ���Ă�����e��
					if (item.owner == first && item.colider == second)
					{
						hasFirstColData = true;
					}
					if (item.owner == second && item.colider == first)
					{
						hasSecondColData = true;
					}
				}
				//�e����Ȃ������ꍇ�����������̃��X�g�ɓ����
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
	//�������������蔻��̓����������̏������Ă�
	for (auto& hitCol : hitData)
	{
		//�X�e�[�W�Ƃ��������蔻����s��Ȃ����̂ł���΃X�L�b�v
		if (hitCol.owner->GetTag() == ObjectTag::kStageObject ||
			hitCol.colider->GetTag() == ObjectTag::kStageObject)
		{
			continue;
		}

		hitCol.OnCollide();
		//����������������̂������W�C�����s��
		if (!hitCol.owner->m_pColData->GetIsTrigger() &&
			!hitCol.colider->m_pColData->GetIsTrigger())
		{
			//���W�C��
			FixNextPosition(hitCol);
		}
	}
	if (m_stageHandle != -1)
	{
		//�X�e�[�W�̓����蔻��
		StageColUpdate();
	}

	//���W�m��
	ConfirmPosition();
}

void Physics::StageColUpdate()
{
	//�����蔻�胊�X�g
	for (auto& item : m_collidables)
	{
		//�������������Ȃ����͔̂��肵�Ȃ�
		if (item->m_isTrigger)return;

		MV1_COLL_RESULT_POLY_DIM hitDim;

		if (item->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{

			auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(item->m_pColData);

			//���͂ɂ���X�e�[�W�|���S�����擾����
			//�ړ��������܂߂Č��o�͈͂��v�Z����
			hitDim = MV1CollCheck_Capsule(m_stageHandle, -1, item->m_rigidbody.GetPos().CastVECTOR(), colData->m_nextEndPos.CastVECTOR(), colData->m_radius + (item->m_nextPos - item->m_rigidbody.GetPos()).Length());
		}
		else if (item->m_pColData->GetKind() == ColliderData::Kind::kSphere)
		{
			auto colData = std::dynamic_pointer_cast<SphereColliderData>(item->m_pColData);

			//���͂ɂ���X�e�[�W�|���S�����擾����
			//�ړ��������܂߂Č��o�͈͂��v�Z����
			hitDim = MV1CollCheck_Sphere(m_stageHandle, -1, item->m_rigidbody.GetPos().CastVECTOR(), colData->m_radius + (item->m_nextPos - item->m_rigidbody.GetPos()).Length());
		}

		m_hitDim = hitDim;

		//�ړ����Ă�����ړ��t���O�𗧂Ă�
		if ((item->m_nextPos - item->m_rigidbody.GetPos()).Length() > kMoveLange)
		{
			item->m_isMove = true;
		}
		else
		{
			item->m_isMove = false;
		}

		//�ǂƏ��̓����蔻����s��
		CheckWallAndFloor(item);
		//�ǂƂ̓����蔻�菈��
		FixPositionWithWall(item);
		//���Ƃ̓����蔻�菈��
		FixNowPositionWithFloor(item);
	}
}

void Physics::SetStage(int stageHandle)
{
	m_stageHandle = stageHandle;
}

void Physics::DebugDraw()
{
	//�����蔻��̕`����s��
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

	////�n�ʂ̐��̕`��
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

	//�����蔻����Ƃ���̃��X�g
	std::list<std::shared_ptr<Collidable>> checkColList;

	//�o�^���Ă��铖���蔻������ׂ�
	for (auto col : m_collidables)
	{
		//�����蔻����Ƃ肽���^�O�Ɠ����ł����
		if (col->GetTag() == tag)
		{
			//�����蔻����Ƃ�悤�ɂ���
			checkColList.push_back(col);
		}
	}

	//�����蔻����Ƃ肽�����̂��J�v�Z���ł����
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
		// Pos���X�V����̂ŁAvelocity�������Ɉړ�����velocity�ɏC��
		MyEngine::Vector3 velocity = item->m_nextPos - item->m_rigidbody.GetPos();
		MyEngine::Vector3 nextPos = item->m_rigidbody.GetPos() + velocity;

		MyEngine::Vector3 centerPos = MyEngine::Vector3(0, 0, 0);

		//�ړ�����������

		//IsTrigger�łȂ����͈̂ړ�������������
		if (!item->m_pColData->GetIsTrigger())
		{
			if ((nextPos - centerPos).Length() > kMoveLimitLength)
			{
				//���W��␳
				nextPos = (nextPos - centerPos).Normalize() * kMoveLimitLength;
			}
			//���������ɍs���Ȃ��悤�ɂ���
			if (nextPos.y < 0.0f)
			{
				nextPos.y = 0.0f;
			}
		}

		//�����蔻�肪�J�v�Z����������
		if (item->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{
			auto capsuleCol = std::dynamic_pointer_cast<CapsuleColliderData>(item->m_pColData);

			//�����ʒu���ꏏ�ɓ������J�v�Z���ł����
			if (capsuleCol->m_isMoveEndPos)
			{
				//�����ʒu���ꏏ�ɓ�����
				capsuleCol->m_endPos = capsuleCol->m_nextEndPos;
			}
		}

		// �ʒu�m��
		item->m_rigidbody.SetPos(nextPos);

	}
}

void Physics::FixNextPosition(OnCollideInfo hitCol)
{
	//���񓮂��������蔻��
	auto moveCol = hitCol.owner;
	//���񓮂��Ȃ������蔻��
	auto staticCol = hitCol.colider;

	//�����������蔻�肪����
	if (moveCol->m_pColData->GetKind() == ColliderData::Kind::kSphere)
	{
		//�����Ȃ������蔻�肪���̏ꍇ
		if (staticCol->m_pColData->GetKind() == ColliderData::Kind::kSphere)
		{
			//���̃f�[�^�Ƀ_�E���L���X�g
			auto moveSphereData = std::dynamic_pointer_cast<SphereColliderData>(moveCol->m_pColData);
			auto staticSphereData = std::dynamic_pointer_cast<SphereColliderData>(staticCol->m_pColData);

			//�����������
			MyEngine::Vector3 fixDir = (moveCol->m_nextPos - staticCol->m_nextPos).Normalize();

			//���̔��a+�������(���a���������ƂԂ����Ă���ꏊ�ɕ␳���邩��)����
			float fixScale = staticSphereData->m_radius + moveSphereData->m_radius + 0.001f;

			//�ʒu�̕␳
			moveCol->m_nextPos = (fixDir * fixScale) + staticCol->m_nextPos;
		}
		//�����Ȃ������蔻�肪�J�v�Z���̏ꍇ
		else if (staticCol->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{
			//�����Ȃ������蔻����J�v�Z���̃f�[�^�Ƀ_�E���L���X�g
			auto staticCapsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(staticCol->m_pColData);

			//���ƃJ�v�Z���̓����蔻����擾����
			float distance = Segment_Point_MinLength(staticCol->m_nextPos.CastVECTOR(), staticCapsuleData->m_nextEndPos.CastVECTOR(), moveCol->m_nextPos.CastVECTOR());

			//�ŋߐړ_�����߂�

			//�J�v�Z���̐�
			MyEngine::Vector3 startToEnd = staticCol->m_nextPos - staticCapsuleData->m_nextEndPos;

			//�J�v�Z���̐��̒[���狅�̍��W�܂ł̃x�N�g��
			MyEngine::Vector3 startToPoint = moveCol->m_nextPos - staticCapsuleData->m_nextEndPos;

			//����̂ǂ̕ӂ������߂�
			float t = startToEnd.Dot(startToPoint) / startToEnd.SqLength();
			//�r������
			t = std::fmax(std::fmin(t, 1.0f), 0.0f);
			//�ŋߐړ_
			MyEngine::Vector3 nearPoint = staticCapsuleData->m_nextEndPos + startToEnd * t;
			//�ŋߐړ_���瓮������̒��S�܂ł̒P�ʃx�N�g��
			MyEngine::Vector3 fixDir = (moveCol->m_nextPos - nearPoint).Normalize();
			//�␳�̑傫��(�J�v�Z���̔��a�������������傫������)
			float fixScale = staticCapsuleData->m_radius + 0.001f;
			//�ʒu�̕␳
			moveCol->m_nextPos = (fixDir * fixScale) + nearPoint;
		}
	}
	//�����������蔻�肪�J�v�Z����
	else if (moveCol->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
	{
		//�����Ȃ������蔻�肪���̏ꍇ
		if (staticCol->m_pColData->GetKind() == ColliderData::Kind::kSphere)
		{
			//���������蔻����J�v�Z���̃f�[�^�Ƀ_�E���L���X�g
			auto moveCapsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(moveCol->m_pColData);

			//�����Ȃ������蔻������̃f�[�^�Ƀ_�E���L���X�g
			auto staticSphereData = std::dynamic_pointer_cast<SphereColliderData>(staticCol->m_pColData);

			//���ƃJ�v�Z���̓����蔻����擾����
			float distance = Segment_Point_MinLength(moveCol->m_nextPos.CastVECTOR(), moveCapsuleData->m_nextEndPos.CastVECTOR(), staticCol->m_nextPos.CastVECTOR());

			//�ŋߐړ_�����߂�

			//�J�v�Z���̐�
			MyEngine::Vector3 startToEnd = moveCol->m_nextPos - moveCapsuleData->m_nextEndPos;

			//�J�v�Z���̐��̒[���狅�̍��W�܂ł̃x�N�g��
			MyEngine::Vector3 startToPoint = staticCol->m_nextPos - moveCapsuleData->m_nextEndPos;

			//����̂ǂ̕ӂ������߂�
			float t = startToEnd.Dot(startToPoint) / startToEnd.SqLength();
			//�r������
			t = std::fmax(std::fmin(t, 1.0f), 0.0f);
			//�ŋߐړ_
			MyEngine::Vector3 nearPoint = moveCapsuleData->m_nextEndPos + startToEnd * t;
			//�����Ȃ�����̒��S����ŋߐړ_�̒P�ʃx�N�g��
			MyEngine::Vector3 fixDir = (nearPoint - staticCol->m_nextPos).Normalize();
			//�␳�̑傫��
			float fixScale = distance - (moveCapsuleData->m_radius + staticSphereData->m_radius) + 0.001f;
			//�ʒu�̕␳
			moveCol->m_nextPos = moveCol->m_nextPos + (fixDir * fixScale);
			moveCapsuleData->m_nextEndPos = moveCapsuleData->m_nextEndPos + (fixDir * fixScale);
		}
		//�����Ȃ������蔻�肪�J�v�Z���̏ꍇ
		else if (staticCol->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{
			//���������蔻����J�v�Z���̃f�[�^�Ƀ_�E���L���X�g
			auto moveCapsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(moveCol->m_pColData);

			//�����Ȃ������蔻����J�v�Z���̃f�[�^�Ƀ_�E���L���X�g
			auto staticCapsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(staticCol->m_pColData);

			//�J�v�Z���̒��S�_���擾����
			MyEngine::Vector3 moveCapsuleCenter = moveCol->m_nextPos + (moveCol->m_nextPos - moveCapsuleData->m_nextEndPos) * 0.5f;
			MyEngine::Vector3 staticCapsuleCenter = staticCol->m_nextPos + (staticCol->m_nextPos - staticCapsuleData->m_nextEndPos) * 0.5f;

			//�����J�v�Z���̐��ƂƓ����Ȃ��J�v�Z���̒��_�̍ŋߐړ_�����߂�
			MyEngine::Vector3 moveCapsuleStartToEnd = moveCol->m_nextPos - moveCapsuleData->m_nextEndPos;
			MyEngine::Vector3 moveCapsuleStartToPoint = staticCapsuleCenter - moveCapsuleData->m_nextEndPos;

			//����̂ǂ̕ӂɍŋߐړ_�����邩
			float moveCapsuleRate = moveCapsuleStartToEnd.Dot(moveCapsuleStartToPoint) / moveCapsuleStartToEnd.SqLength();
			//�r������
			moveCapsuleRate = std::fmax(std::fmin(moveCapsuleRate, 1.0f), 0.0f);

			//���ƒ��_�̍ŋߐړ_
			MyEngine::Vector3 moveNearestPoint = moveCapsuleData->m_nextEndPos + moveCapsuleStartToEnd * moveCapsuleRate;

			//���ƒ��_�̍ŋߐړ_�Ɠ����Ȃ��J�v�Z���̐��̍ŋߐړ_�����߂�
			MyEngine::Vector3 staticCapsuleStartToEnd = staticCol->m_nextPos - staticCapsuleData->m_nextEndPos;
			MyEngine::Vector3 staticCapsuleStartToPoint = moveNearestPoint - staticCapsuleData->m_nextEndPos;

			//����̂ǂ̕ӂɍŋߐړ_�����邩
			float staticCapsuleRate = staticCapsuleStartToEnd.Dot(staticCapsuleStartToPoint) / staticCapsuleStartToEnd.SqLength();

			//�r������
			staticCapsuleRate = std::fmax(std::fmin(staticCapsuleRate, 1.0f), 0.0f);

			//�ŋߐړ_
			MyEngine::Vector3 staticNearestPoint = staticCapsuleData->m_nextEndPos + staticCapsuleStartToEnd * staticCapsuleRate;



			DrawSphere3D(staticNearestPoint.CastVECTOR(), 2, 3, GetColor(255, 0, 0), GetColor(255, 0, 0), true);

			//�␳��������x�N�g��
			MyEngine::Vector3 fixDir = (moveNearestPoint - staticNearestPoint).Normalize();

			//�ŋߐړ_�̋������擾
			float distance = (moveNearestPoint - staticNearestPoint).Length();

			//�␳����傫��
			float fixScale = moveCapsuleData->m_radius + staticCapsuleData->m_radius + 0.01f - distance;

			moveCol->m_nextPos = moveCol->m_nextPos + (fixDir * fixScale);
			moveCapsuleData->m_nextEndPos = moveCapsuleData->m_nextEndPos + (fixDir * fixScale);

		}
	}
}

void Physics::CheckWallAndFloor(std::shared_ptr<Collidable> collider)
{
	//�ǃ|���S���Ə��|���S���̐�������������
	collider->m_wallNum = 0;
	collider->m_floorNum = 0;

	//���o���ꂽ�|���S���̐������J��Ԃ�
	for (int i = 0; i < m_hitDim.HitNum; i++)
	{
		//�|���S���̖@����Y�������ǃ|���S���Ɣ��f����l�ɒB���Ă��邩�ǂ����ŕǂ��������f����
		if (m_hitDim.Dim[i].Normal.y < kWallPolyYDir && m_hitDim.Dim[i].Normal.y > -kWallPolyYDir)
		{
			// �ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�����蔻��̂x���W�����l�����|���S���̂ݓ����蔻����s��
			// �K�i��̂��̂ȂǂɈ���������Ȃ��悤�ɂ��邽�߂�if��
			if (m_hitDim.Dim[i].Position[0].y > collider->m_rigidbody.GetPos().y + kWallHeight ||
				m_hitDim.Dim[i].Position[1].y > collider->m_rigidbody.GetPos().y + kWallHeight ||
				m_hitDim.Dim[i].Position[2].y > collider->m_rigidbody.GetPos().y + kWallHeight)
			{
				//�|���S���̐������E���ɒB���Ă��Ȃ���΃|���S����z��ɒǉ�
				if (collider->m_wallNum < kMaxColHitPolyNum)
				{
					//�|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
					collider->m_pWallPoly[collider->m_wallNum] = &m_hitDim.Dim[i];

					//�ǃ|���S���̐������Z����
					collider->m_wallNum++;
				}
			}
		}
		else
		{
			//�|���S���̐������E���ɒB���Ă��Ȃ���΃|���S����z��ɒǉ�
			if (collider->m_floorNum < kMaxColHitPolyNum)
			{
				//�|���S���̍\���̂̃A�h���X�����|���S���̔z��ɒǉ�����
				collider->m_pFloorPoly[collider->m_floorNum] = &m_hitDim.Dim[i];

				//���|���S���̐������Z����
				collider->m_floorNum++;
			}
		}
	}
}

void Physics::FixPositionWithWall(std::shared_ptr<Collidable> collider)
{
	//�ǃ|���S�����Ȃ��ꍇ�͉������Ȃ�
	if (collider->m_wallNum == 0)
	{
		//�ǂɓ������Ă��Ȃ��t���O�𗧂Ă�
		collider->m_pColData->SetIsWall(false);

		return;
	}
	//�ǃ|���S���Ƃ̓����蔻��
	//�X�e�[�W�ɓ����������ǂ����t���O��false�ɂ��Ă���
	collider->m_isHitPoly = false;

	auto colKind = collider->m_pColData->GetKind();


	//�ړ��������ǂ����ŏ����𕪊򂳂���
	if (collider->m_isMove)
	{
		//�����蔻��̎�ނɂ���ď����𕪊򂳂���
		if (colKind == ColliderData::Kind::kCapsule)
		{
			auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(collider->m_pColData);

			//�ǃ|���S���̐������J��Ԃ�
			for (int i = 0; i < collider->m_wallNum; i++)
			{
				//i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
				m_pPoly = collider->m_pWallPoly[i];
				//�|���S���Ɠ����蔻�肪�������Ă��Ȃ���Ύ��̃J�E���g��
				if (!HitCheck_Capsule_Triangle(colData->m_nextEndPos.CastVECTOR(), collider->m_nextPos.CastVECTOR(), colData->m_radius,
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))continue;

				//�����ɗ����瓖�����Ă���Ƃ������ƂȂ̂œ������Ă���t���O�𗧂Ă�
				collider->m_isHitPoly = true;

				//�ǂ��l�������ړ����O�ς��g���Čv�Z
				MyEngine::Vector3 slideVec;

				//�i�s�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����v�Z
				slideVec = (collider->m_nextPos - collider->m_rigidbody.GetPos()).Cross(m_pPoly->Normal);

				//�v�Z�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g��
				//���̈ړ���������Ǖ����̈ړ������𔲂����x�N�g��
				slideVec = MyEngine::Vector3(m_pPoly->Normal).Cross(slideVec);

				//������ړ��O�̍��W�ɑ��������̂�V���ȍ��W�ɂ���
				collider->m_nextPos = collider->m_nextPos + slideVec;

			}
		}
		else if (colKind == ColliderData::Kind::kSphere)
		{
			auto colData = std::dynamic_pointer_cast<SphereColliderData>(collider->m_pColData);
			//�ǃ|���S���̐������J��Ԃ�
			for (int i = 0; i < collider->m_wallNum; i++)
			{
				//i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
				m_pPoly = collider->m_pWallPoly[i];
				//�|���S���Ɠ����蔻�肪�������Ă��Ȃ���Ύ��̃J�E���g��
				if (!HitCheck_Sphere_Triangle(collider->m_nextPos.CastVECTOR(), colData->m_radius,
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))continue;

				//�����ɗ����瓖�����Ă���Ƃ������ƂȂ̂œ������Ă���t���O�𗧂Ă�
				collider->m_isHitPoly = true;

				//�ǂ��l�������ړ����O�ς��g���Čv�Z
				MyEngine::Vector3 slideVec;

				//�i�s�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����v�Z
				slideVec = (collider->m_nextPos - collider->m_rigidbody.GetPos()).Cross(m_pPoly->Normal);

				//�v�Z�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g��
				//���̈ړ���������Ǖ����̈ړ������𔲂����x�N�g��
				slideVec = MyEngine::Vector3(m_pPoly->Normal).Cross(slideVec);

				//������ړ��O�̍��W�ɑ��������̂�V���ȍ��W�ɂ���
				collider->m_nextPos = collider->m_nextPos + slideVec;
			}
		}
	}
	else
	{
		//�ړ����Ă��Ȃ��ꍇ�̏���

		//�ǃ|���S���̐������J��Ԃ�
		for (int i = 0; i < collider->m_wallNum; i++)
		{
			//i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���z�񂩂�擾
			m_pPoly = collider->m_pWallPoly[i];

			//�|���S���ɓ������Ă����瓖�������t���O�𗧂Ă������Ń��[�v���甲����

			//�����蔻��̎�ނɂ���ď����𕪊򂳂���
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

	//�ǂɓ������Ă�����ǂ��牟���o���������s��
	if (collider->m_isHitPoly)
	{
		FixPositionWithWallInternal(collider);

		collider->m_pColData->SetIsWall(true);
	}
}

void Physics::FixPositionWithWallInternal(std::shared_ptr<Collidable> collider)
{
	//�ǂ��牟���o������������ő吔�����J��Ԃ�
	for (int i = 0; i < kMaxPushWallNum; i++)
	{
		//�������Ă���\���̂���ǃ|���S�������ׂČ���
		bool isHitWall = false;

		//�����蔻��̎��
		auto colKind = collider->m_pColData->GetKind();


		//�����蔻��̎�ނɂ���ď����𕪊򂳂���
		if (colKind == ColliderData::Kind::kCapsule)
		{
			auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(collider->m_pColData);

			//�ǃ|���S���̐������J��Ԃ�
			for (int j = 0; j < collider->m_wallNum; j++)
			{
				//j�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
				m_pPoly = collider->m_pWallPoly[j];


				//�����蔻��Ɠ������Ă��邩�𔻒�
				if (!HitCheck_Capsule_Triangle(collider->m_nextPos.CastVECTOR(), (collider->m_nextPos + colData->m_lange).CastVECTOR(), colData->m_radius,
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))continue;

				//�������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
				collider->m_nextPos = collider->m_nextPos + (MyEngine::Vector3(m_pPoly->Normal) * kColHitSlideLength);

				//�ړ����������ŕǃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
				for (int k = 0; k < collider->m_wallNum; k++)
				{
					//�������Ă����烋�[�v�𔲂���
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
			//�ǃ|���S���̐������J��Ԃ�
			for (int j = 0; j < collider->m_wallNum; j++)
			{
				//j�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
				m_pPoly = collider->m_pWallPoly[j];


				//�����蔻��Ɠ������Ă��邩�𔻒�
				if (!HitCheck_Sphere_Triangle(collider->m_nextPos.CastVECTOR(), colData->m_radius,
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))continue;

				//�������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
				collider->m_nextPos = collider->m_nextPos + (MyEngine::Vector3(m_pPoly->Normal) * kColHitSlideLength);

				//�ړ����������ŕǃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
				for (int k = 0; k < collider->m_wallNum; k++)
				{
					//�������Ă����烋�[�v�𔲂���
					m_pPoly = collider->m_pWallPoly[k];
					if (HitCheck_Sphere_Triangle(collider->m_nextPos.CastVECTOR(), colData->m_radius,
						m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
					{
						collider->m_isHitPoly = true;
						break;
					}
				}
			}

			//���ׂẴ|���S���Ɠ������Ă��Ȃ���΃��[�v�I��
			if (!collider->m_isHitPoly)break;
		}
		//���[�v�I��
		if (!collider->m_isHitPoly)break;
	}
}

void Physics::FixNowPositionWithFloor(std::shared_ptr<Collidable> collider)
{
	//���|���S�����Ȃ��ꍇ
	if (collider->m_floorNum == 0)
	{
		//���ƂԂ����Ă��邩�ǂ�����false�ɂ��Ă���
		collider->m_pColData->SetIsGround(false);
		return;
	}

	//���|���S���Ƃ̓����蔻�菈��

	//�����������ǂ����̃t���O������
	bool isHitFlag = false;



	//���̂��㏸���Ă�����
	if ((collider->m_nextPos.y - collider->m_rigidbody.GetPos().y) > 0)
	{
		//�V��ɂԂ��鏈�����s��
		//��ԒႢ�V��ɂԂ��邽�߂ɏ�����
		float polyMinPosY = 0.0f;

		auto colKind = collider->m_pColData->GetKind();

		if (colKind == ColliderData::Kind::kCapsule)
		{

			auto capsule = std::dynamic_pointer_cast<CapsuleColliderData>(collider->m_pColData);

			//���̃|���S���̐������J��Ԃ�
			for (int i = 0; i < collider->m_floorNum; i++)
			{
				//i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
				m_pPoly = collider->m_pFloorPoly[i];

				//�n�_����I�_�܂ł̊�
				m_lineRes = HitCheck_Line_Triangle(collider->m_nextPos.CastVECTOR(), (collider->m_nextPos + capsule->m_lange).CastVECTOR(),
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]);

				//�ڐG���Ă��Ȃ���Ή������Ȃ�
				if (!m_lineRes.HitFlag)continue;

				//�V��|���S�������܂Ō��o���ꂽ�|���S�����Ⴂ�ꍇ������ʂ�
				if (polyMinPosY < m_lineRes.Position.y)
				{
					//�|���S���ɓ��������t���O�𗧂Ă�
					isHitFlag = true;

					//�ڐG����Y���W�ۑ�����
					polyMinPosY = m_lineRes.Position.y;
				}

			}
		}
		else if (colKind == ColliderData::Kind::kSphere)
		{
			auto sphere = std::dynamic_pointer_cast<SphereColliderData>(collider->m_pColData);

			//���̃|���S���̐������J��Ԃ�
			for (int i = 0; i < collider->m_floorNum; i++)
			{
				//i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
				m_pPoly = collider->m_pFloorPoly[i];

				VECTOR nextPos = collider->m_nextPos.CastVECTOR();

				//�_�Ƃ̓����蔻��
				m_lineRes = HitCheck_Line_Triangle(collider->m_nextPos.CastVECTOR(), (collider->m_nextPos + MyEngine::Vector3(0.1f, 0.0f, 0.0f)).CastVECTOR(),
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]);

				//�������Ă��Ȃ���Ώ��������Ȃ�
				if (!m_lineRes.HitFlag)continue;

				//�V��|���S�������܂Ō��o���ꂽ�|���S�����Ⴂ�ꍇ������ʂ�
				if (polyMinPosY < m_lineRes.Position.y)
				{
					//�|���S���ɓ��������t���O�𗧂Ă�
					isHitFlag = true;

					//�ڐG����Y���W�ۑ�����
					polyMinPosY = m_lineRes.Position.y;
				}

			}
		}

		//�ڐG�����|���S���������
		if (isHitFlag)
		{
			if (colKind == ColliderData::Kind::kCapsule)
			{
				auto capsule = std::dynamic_pointer_cast<CapsuleColliderData>(collider->m_pColData);

				//�ڐG�����ꍇ�̓v���C���[��Y���W��ڐG���W�����ƂɍX�V
				collider->m_nextPos.y = polyMinPosY - capsule->m_radius;
			}
			else if (colKind == ColliderData::Kind::kSphere)
			{
				auto sphere = std::dynamic_pointer_cast<SphereColliderData>(collider->m_pColData);

				//�ڐG�����ꍇ�̓v���C���[��Y���W��ڐG���W�����ƂɍX�V
				collider->m_nextPos.y = polyMinPosY - sphere->m_radius;
			}
		}
	}
	//���|���S���Ƃ̓����蔻��
	else
	{
		//��ԍ������|���S���ɂԂ��邽�߂̔���p�ϐ���������
		float polyMaxPosY = 0.0f;

		//���|���S���ɓ����������ǂ����̃t���O��false�ɂ��Ă���
		collider->m_isHitPoly = false;
		auto colKind = collider->m_pColData->GetKind();
		if (colKind == ColliderData::Kind::kCapsule)
		{

			auto capsule = std::dynamic_pointer_cast<CapsuleColliderData>(collider->m_pColData);

			//���̃|���S���̐������J��Ԃ�
			for (int i = 0; i < collider->m_floorNum; i++)
			{
				//i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
				m_pPoly = collider->m_pFloorPoly[i];

				//�n�_����I�_�܂ł̊�
				m_lineRes = HitCheck_Line_Triangle((collider->m_nextPos + capsule->m_lange).CastVECTOR(), collider->m_nextPos.CastVECTOR(),
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]);

				//�ڐG���Ă��Ȃ���Ή������Ȃ�
				if (!m_lineRes.HitFlag)continue;

				//���łɓ��������|���S��������A�����܂Ō��o�������|���S�����Ⴂ�ꍇ�͉������Ȃ�
				if (collider->m_isHitPoly && polyMaxPosY > m_lineRes.Position.y) continue;

				//�|���S���ɓ��������t���O�𗧂Ă�
				collider->m_isHitPoly = true;

				//�ڐG����Y���W�ۑ�����
				polyMaxPosY = m_lineRes.Position.y;

			}
		}
		else if (colKind == ColliderData::Kind::kSphere)
		{
			auto sphere = std::dynamic_pointer_cast<SphereColliderData>(collider->m_pColData);

			//���̃|���S���̐������J��Ԃ�
			for (int i = 0; i < collider->m_floorNum; i++)
			{
				//i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
				m_pPoly = collider->m_pFloorPoly[i];

				VECTOR nextPos = collider->m_nextPos.CastVECTOR();

				//�_�Ƃ̓����蔻��
				m_lineRes = HitCheck_Line_Triangle((collider->m_nextPos + MyEngine::Vector3(0.0f, sphere->m_radius, 0.0f)).CastVECTOR(), (collider->m_nextPos + MyEngine::Vector3(0.0f, -sphere->m_radius, 0.0f)).CastVECTOR(),
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]);

				//�ڐG���Ă��Ȃ���Ή������Ȃ�
				if (!m_lineRes.HitFlag)continue;

				//���łɓ��������|���S��������A�����܂Ō��o�������|���S�����Ⴂ�ꍇ�͉������Ȃ�
				if (collider->m_isHitPoly && polyMaxPosY > m_lineRes.Position.y) continue;

				//�|���S���ɓ��������t���O�𗧂Ă�
				collider->m_isHitPoly = true;

				//�ڐG����Y���W�ۑ�����
				polyMaxPosY = m_lineRes.Position.y;

			}
		}


		//�ڐG�����|���S���������
		if (collider->m_isHitPoly)
		{
			if (collider->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
			{
				auto capsule = std::dynamic_pointer_cast<CapsuleColliderData>(collider->m_pColData);

				//�ڐG�����|���S���ň�ԍ���Y���W�𓖂��蔻���Y���W�ɂ���
				collider->m_nextPos.y = polyMaxPosY + capsule->m_radius;

				capsule->m_nextEndPos.y = polyMaxPosY + capsule->m_radius + capsule->m_lange.y;
			}
			else if (collider->m_pColData->GetKind() == ColliderData::Kind::kSphere)
			{
				auto sphere = std::dynamic_pointer_cast<SphereColliderData>(collider->m_pColData);

				collider->m_nextPos.y = polyMaxPosY + sphere->m_radius;
			}

			//���ɓ������Ă���t���O�𗧂Ă�
			collider->m_pColData->SetIsGround(true);
		}
	}
}

bool Physics::IsCheckCollide(std::shared_ptr<Collidable> first, std::shared_ptr<Collidable> second)
{
	//���̓����蔻��Ƒ��̓����蔻�肪���Ȃ����̂łȂ����
	if (first != second)
	{
		//�����蔻��̎�ނ��擾
		ColliderData::Kind firstKind = first->m_pColData->GetKind();
		ColliderData::Kind secondKind = second->m_pColData->GetKind();


		//���ǂ����̓����蔻��
		if (firstKind == ColliderData::Kind::kSphere && secondKind == ColliderData::Kind::kSphere)
		{
			//�����蔻������Ƀ_�E���L���X�g����
			auto firstCol = std::dynamic_pointer_cast<SphereColliderData>(first->m_pColData);
			auto secondCol = std::dynamic_pointer_cast<SphereColliderData>(second->m_pColData);

			//�����蔻��̋��������߂�
			float distance = (first->m_nextPos - second->m_nextPos).Length();

			//���̑傫�������킹�����̂����������Z����΂Ԃ����Ă���
			if (distance < firstCol->m_radius + secondCol->m_radius)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		//�J�v�Z�����m�̓����蔻��
		else if (firstKind == ColliderData::Kind::kCapsule && secondKind == ColliderData::Kind::kCapsule)
		{
			//�����蔻����J�v�Z���Ƀ_�E���L���X�g����
			auto firstCol = std::dynamic_pointer_cast<CapsuleColliderData>(first->m_pColData);
			auto secondCol = std::dynamic_pointer_cast<CapsuleColliderData>(second->m_pColData);

			//�J�v�Z�����m�̍ŒZ����
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
		//���ƃJ�v�Z���̓����蔻��
		else if (firstKind == ColliderData::Kind::kSphere && secondKind == ColliderData::Kind::kCapsule ||
			firstKind == ColliderData::Kind::kCapsule && secondKind == ColliderData::Kind::kSphere)
		{
			//���ƃJ�v�Z���̃R���C�_�[�f�[�^�쐬
			std::shared_ptr<ColliderData> sphereDataBase;
			std::shared_ptr<ColliderData> capsuleDataBase;
			float distance;
			//�ǂ��炪�J�v�Z���Ȃ̂����Ȃ̂����ʂ��Ă���f�[�^������
			if (firstKind == ColliderData::Kind::kSphere)
			{
				sphereDataBase = first->m_pColData;
				capsuleDataBase = second->m_pColData;

				//�J�v�Z���̐����̃f�[�^���g�����߂Ƀ_�E���L���X�g
				auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleDataBase);
				//�����Ɠ_�̍ŋߋ��������߂�
				distance = Segment_Point_MinLength(second->m_nextPos.CastVECTOR(),
					capsuleData->m_nextEndPos.CastVECTOR(), first->m_nextPos.CastVECTOR());
			}
			else
			{
				capsuleDataBase = first->m_pColData;
				sphereDataBase = second->m_pColData;
				//�J�v�Z���̐����̃f�[�^���g�����߂Ƀ_�E���L���X�g
				auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleDataBase);
				//�����Ɠ_�̍ŋߋ��������߂�
				distance = Segment_Point_MinLength(first->m_nextPos.CastVECTOR(),
					capsuleData->m_nextEndPos.CastVECTOR(), second->m_nextPos.CastVECTOR());
			}
			//�_�E���L���X�g
			auto sphereData = std::dynamic_pointer_cast<SphereColliderData>(sphereDataBase);
			auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleDataBase);

			//�~�ƃJ�v�Z���̔��a�����~�ƃJ�v�Z���̋������߂���Γ������Ă���
			if (distance < sphereData->m_radius + capsuleData->m_radius)
			{
				return true;
			}
			else
			{
				return false;
			}


		}
		//�ǂ�ɂ����Ă͂܂�Ȃ�������
		else
		{
			assert(false);
			return false;
		}
	}
	return false;
}
