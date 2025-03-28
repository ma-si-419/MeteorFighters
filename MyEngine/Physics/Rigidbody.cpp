#include "Rigidbody.h"

Rigidbody::Rigidbody() :
	m_pos(),
	m_dir(),
	m_velo()
{
}

void Rigidbody::Init()
{
	m_pos = MyEngine::Vector3(0,0,0);
	m_dir = MyEngine::Vector3(0,0,0);
	m_velo = MyEngine::Vector3(0,0,0);
}

void Rigidbody::SetVelo(MyEngine::Vector3 velo)
{
	m_velo = velo;
	//移動量があったら
	if (m_velo.SqLength() > 0)
	{
		//方向ベクトルを設定する
		m_dir = m_velo.Normalize();
	}
}
