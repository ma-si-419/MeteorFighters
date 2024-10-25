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
	//������
	for (auto& item : m_collidables)
	{
		item->m_nextPos = (item->m_rigidbody.GetPos() + item->m_rigidbody.GetVelo());

		//�����蔻�肪�J�v�Z����������
		if (item->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{
			auto capsuleCol = std::dynamic_pointer_cast<CapsuleColliderData>(item->m_pColData);

			//�����ʒu���ꏏ�ɓ������J�v�Z���ł����
			if (capsuleCol->m_isMoveStartPos)
			{
				//�����ʒu���ꏏ�ɓ�����
				MyEngine::Vector3 pos = item->m_nextPos;
				pos.y += capsuleCol->m_length;
				capsuleCol->m_nextStartPos = pos;

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
					if (item.owner == first)
					{
						hasFirstColData = true;
					}
					if (item.owner == second)
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
		hitCol.OnCollide();
		//����������������̂������W�C�����s��
		if (!hitCol.owner->m_pColData->GetIsTrigger() ||
			!hitCol.colider->m_pColData->GetIsTrigger())
		{
			//���W�C��
			FixNextPosition(hitCol);
		}
	}
	//���W�m��
	ConfirmPosition();
}

void Physics::DebugDraw()
{
	//�����蔻��̕`����s��
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

	//�n�ʂ̐��̕`��
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
		// Pos���X�V����̂ŁAvelocity�������Ɉړ�����velocity�ɏC��
		MyEngine::Vector3 velocity = item->m_nextPos - item->m_rigidbody.GetPos();
		MyEngine::Vector3 nextPos = item->m_rigidbody.GetPos() + velocity;

		//�����蔻�肪�J�v�Z����������
		if (item->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{
			auto capsuleCol = std::dynamic_pointer_cast<CapsuleColliderData>(item->m_pColData);

			//�����ʒu���ꏏ�ɓ������J�v�Z���ł����
			if (capsuleCol->m_isMoveStartPos)
			{
				//�����ʒu���ꏏ�ɓ�����
				capsuleCol->m_startPos = capsuleCol->m_nextStartPos;
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
			auto staticSphereData = std::dynamic_pointer_cast<SphereColliderData>(staticCol->m_pColData);

			//�����������
			MyEngine::Vector3 fixDir = (moveCol->m_nextPos - staticCol->m_nextPos).Normalize();

			//���̔��a+�������(���a���������ƂԂ����Ă���ꏊ�ɕ␳���邩��)����
			float fixScale = staticSphereData->m_radius + 0.001f;

			//�ʒu�̕␳
			moveCol->m_nextPos = (fixDir * fixScale) + staticCol->m_nextPos;
		}
		//�����Ȃ������蔻�肪�J�v�Z���̏ꍇ
		else if (staticCol->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{
			//�����Ȃ������蔻����J�v�Z���̃f�[�^�Ƀ_�E���L���X�g
			auto staticCapsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(staticCol->m_pColData);

			//���ƃJ�v�Z���̓����蔻����擾����
			float distance = Segment_Point_MinLength(staticCol->m_nextPos.CastVECTOR(), staticCapsuleData->m_nextStartPos.CastVECTOR(), moveCol->m_nextPos.CastVECTOR());

			//�ŋߐړ_�����߂�

			//�J�v�Z���̐�
			MyEngine::Vector3 startToEnd = staticCol->m_nextPos - staticCapsuleData->m_nextStartPos;

			//�J�v�Z���̐��̒[���狅�̍��W�܂ł̃x�N�g��
			MyEngine::Vector3 startToPoint = moveCol->m_nextPos - staticCapsuleData->m_nextStartPos;

			//����̂ǂ̕ӂ������߂�
			float t = startToEnd.Dot(startToPoint) / startToEnd.SqLength();
			//�r������
			t = std::fmax(std::fmin(t, 1.0f), 0.0f);
			//�ŋߐړ_
			MyEngine::Vector3 nearPoint = staticCapsuleData->m_nextStartPos + startToEnd * t;
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
			float distance = Segment_Point_MinLength(moveCol->m_nextPos.CastVECTOR(), moveCapsuleData->m_nextStartPos.CastVECTOR(), staticCol->m_nextPos.CastVECTOR());

			//�ŋߐړ_�����߂�

			//�J�v�Z���̐�
			MyEngine::Vector3 startToEnd = moveCol->m_nextPos - moveCapsuleData->m_nextStartPos;

			//�J�v�Z���̐��̒[���狅�̍��W�܂ł̃x�N�g��
			MyEngine::Vector3 startToPoint = staticCol->m_nextPos - moveCapsuleData->m_nextStartPos;

			//����̂ǂ̕ӂ������߂�
			float t = startToEnd.Dot(startToPoint) / startToEnd.SqLength();
			//�r������
			t = std::fmax(std::fmin(t, 1.0f), 0.0f);
			//�ŋߐړ_
			MyEngine::Vector3 nearPoint = moveCapsuleData->m_nextStartPos + startToEnd * t;
			//�����Ȃ�����̒��S����ŋߐړ_�̒P�ʃx�N�g��
			MyEngine::Vector3 fixDir = (nearPoint - staticCol->m_nextPos).Normalize();
			//�␳�̑傫��
			float fixScale = distance - (moveCapsuleData->m_radius + staticSphereData->m_radius) + 0.001f;
			//�ʒu�̕␳
			moveCol->m_nextPos = moveCol->m_nextPos + (fixDir * fixScale);
			moveCapsuleData->m_nextStartPos = moveCapsuleData->m_nextStartPos + (fixDir * fixScale);
		}
		//�����Ȃ������蔻�肪�J�v�Z���̏ꍇ
		else if (staticCol->m_pColData->GetKind() == ColliderData::Kind::kCapsule)
		{
			//���������蔻����J�v�Z���̃f�[�^�Ƀ_�E���L���X�g
			auto moveCapsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(moveCol->m_pColData);

			//�����Ȃ������蔻����J�v�Z���̃f�[�^�Ƀ_�E���L���X�g
			auto staticCapsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(staticCol->m_pColData);

			//�J�v�Z���̒��S�_���擾����
			MyEngine::Vector3 moveCapsuleCenter = moveCol->m_nextPos + (moveCol->m_nextPos - moveCapsuleData->m_nextStartPos) * 0.5f;
			MyEngine::Vector3 staticCapsuleCenter = staticCol->m_nextPos + (staticCol->m_nextPos - staticCapsuleData->m_nextStartPos) * 0.5f;
			
			//�����J�v�Z���̐��ƂƓ����Ȃ��J�v�Z���̒��_�̍ŋߐړ_�����߂�
			MyEngine::Vector3 moveCapsuleStartToEnd = moveCol->m_nextPos - moveCapsuleData->m_nextStartPos;
			MyEngine::Vector3 moveCapsuleStartToPoint = staticCapsuleCenter - moveCapsuleData->m_nextStartPos;
			
			//����̂ǂ̕ӂɍŋߐړ_�����邩
			float moveCapsuleRate = moveCapsuleStartToEnd.Dot(moveCapsuleStartToPoint) / moveCapsuleStartToEnd.SqLength();
			//�r������
			moveCapsuleRate = std::fmax(std::fmin(moveCapsuleRate,1.0f),0.0f);

			//���ƒ��_�̍ŋߐړ_
			MyEngine::Vector3 moveNearestPoint = moveCapsuleData->m_nextStartPos + moveCapsuleStartToEnd * moveCapsuleRate;

			//���ƒ��_�̍ŋߐړ_�Ɠ����Ȃ��J�v�Z���̐��̍ŋߐړ_�����߂�
			MyEngine::Vector3 staticCapsuleStartToEnd = staticCol->m_nextPos - staticCapsuleData->m_nextStartPos;
			MyEngine::Vector3 staticCapsuleStartToPoint = moveNearestPoint - staticCapsuleData->m_nextStartPos;

			//����̂ǂ̕ӂɍŋߐړ_�����邩
			float staticCapsuleRate = staticCapsuleStartToEnd.Dot(staticCapsuleStartToPoint) / staticCapsuleStartToEnd.SqLength();

			//�r������
			staticCapsuleRate = std::fmax(std::fmin(staticCapsuleRate, 1.0f), 0.0f);

			//�ŋߐړ_
			MyEngine::Vector3 staticNearestPoint = staticCapsuleData->m_nextStartPos + staticCapsuleStartToEnd * staticCapsuleRate;
			
			

			DrawSphere3D(staticNearestPoint.CastVECTOR(),2,3,GetColor(255,0,0),GetColor(255,0,0),true);

			//�␳��������x�N�g��
			MyEngine::Vector3 fixDir = (moveNearestPoint - staticNearestPoint).Normalize();

			//�ŋߐړ_�̋������擾
			float distance = (moveNearestPoint - staticNearestPoint).Length();

			//�␳����傫��
			float fixScale = moveCapsuleData->m_radius + staticCapsuleData->m_radius + 0.01f - distance;

			moveCol->m_nextPos = moveCol->m_nextPos + (fixDir * fixScale);
			moveCapsuleData->m_nextStartPos = moveCapsuleData->m_nextStartPos + (fixDir * fixScale);

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
					capsuleData->m_nextStartPos.CastVECTOR(), first->m_nextPos.CastVECTOR());
			}
			else
			{
				capsuleDataBase = first->m_pColData;
				sphereDataBase = second->m_pColData;
				//�J�v�Z���̐����̃f�[�^���g�����߂Ƀ_�E���L���X�g
				auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleDataBase);
				//�����Ɠ_�̍ŋߋ��������߂�
				distance = Segment_Point_MinLength(first->m_nextPos.CastVECTOR(),
					capsuleData->m_nextStartPos.CastVECTOR(), second->m_nextPos.CastVECTOR());
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
