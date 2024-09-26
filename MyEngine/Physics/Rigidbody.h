#pragma once
#include "Vector3.h"
class Rigidbody
{
public:
	Rigidbody();
	void Init();

	MyEngine::Vector3 GetPos() const { return m_pos; }
	MyEngine::Vector3 GetDir() const { return m_dir; }
	MyEngine::Vector3 GetVelo() const { return m_velo; }

	void SetPos(MyEngine::Vector3 pos) { m_pos = pos; }
	void SetVelo(MyEngine::Vector3 velo);
	MyEngine::Vector3 GetPos() { return m_pos; }

private:
	MyEngine::Vector3 m_pos;
	MyEngine::Vector3 m_dir;
	MyEngine::Vector3 m_velo;
};