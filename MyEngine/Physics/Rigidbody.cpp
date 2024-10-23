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
	//ˆÚ“®—Ê‚ª‚ ‚Á‚½‚ç
	if (m_velo.SqLength() > 0)
	{
		//•ûŒüƒxƒNƒgƒ‹‚ğİ’è‚·‚é
		m_dir = m_velo.Normalize();
	}
}
