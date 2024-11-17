#pragma once
#include "Vector3.h"

namespace MyEngine
{

	struct LocalPos
	{

		LocalPos();

		/// <summary>
		/// ���[�J�����W�����[���h���W�ɕϊ�
		/// </summary>
		/// <returns>���[���h���W</returns>
		MyEngine::Vector3 GetWorldPos();

		/// <summary>
		/// ���S���W���擾����
		/// </summary>
		/// <returns>���S�Ƃ��Ă�����W</returns>
		MyEngine::Vector3 GetCenterPos() { return m_centerPos; };

		/// <summary>
		/// ���S�Ƃ�����W��ݒ�
		/// </summary>
		/// <param name="pos">���S���W(���[���h���W)</param>
		void SetCenterPos(MyEngine::Vector3 pos);

		/// <summary>
		/// ���[�J�����W��ݒ�
		/// </summary>
		/// <param name="pos">���S����̃x�N�g��</param>
		void SetLocalPos(MyEngine::Vector3 pos);

		/// <summary>
		/// �ݒ肵�Ă��郍�[�J�����W���擾����
		/// </summary>
		/// <returns>���[�J�����W</returns>
		MyEngine::Vector3 GetLocalPos() { return m_localPos; }

		/// <summary>
		/// ���S�̌����Ă��������ݒ肷��
		/// </summary>
		/// <param name="frontPos">���S���W�̑O�����Ƃ�����W</param>
		void SetFrontPos(MyEngine::Vector3 frontPos);

		/// <summary>
		/// ���[���h���W�����[�J�����W�ɕϊ�
		/// </summary>
		/// <param name="worldPos">�ϊ����������[���h���W</param>
		/// <returns>�ϊ���̃��[�J�����W</returns>
		MyEngine::Vector3 ChangeWorldToLocal(MyEngine::Vector3 worldPos);

	private:

		MyEngine::Vector3 m_centerPos;
		MyEngine::Vector3 m_centerRotation;

		MyEngine::Vector3 m_localPos;

	};

}