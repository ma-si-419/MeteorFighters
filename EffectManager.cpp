#include "EffectManager.h"
#include "Effect.h"
#include "EffekseerForDXLib.h"
#include "Effekseer.h"

void EffectManager::Entry(std::shared_ptr<Effect> effect, MyEngine::Vector3 pos)
{
	int handle = -1;

	//find��key��T���āA������Ȃ������ꍇend�̈ʒu��Ԃ�����
	//find�̕Ԃ�l��end�ƈقȂ����猩�������Ƃ������ƁAend�ƈ�v�����猩����Ȃ�����(false)
	auto find = m_handles.find(effect->GetPath()) != m_handles.end();

	//���������ꍇ
	if (find)
	{
		//�o�^���Ă���n���h����Ԃ�
		handle = m_handles[effect->GetPath()];
	}
	//������Ȃ������ꍇ
	else
	{
		//�n���h�������[�h����
		handle = LoadEffekseerEffect(effect->GetPath().c_str(), 1.0f);

		//�n���h�������[�h�ł��Ă�����
		if (handle != -1)
		{
			//�n���h����o�^����
			m_handles[effect->GetPath()] = handle;
		}
	}

	int playHandle = PlayEffekseer3DEffect(handle);

	printfDx("�v���C\n");

	SetPosPlayingEffekseer3DEffect(playHandle, pos.x, pos.y, pos.z);

	effect->SetHandle(playHandle);

	m_effects.push_back(effect);
}

void EffectManager::Exit(std::shared_ptr<Effect> effect)
{
	bool find = (std::find(m_effects.begin(), m_effects.end(), effect) != m_effects.end());
	//�o�^����Ă����ꍇ
	if (find)
	{
		StopEffekseer3DEffect(effect->GetHandle());

		m_effects.remove(effect);
	}
}

void EffectManager::Update()
{
	UpdateEffekseer3D();

	std::vector<std::shared_ptr<Effect>> deleteEffects;
	for (auto& item : m_effects)
	{
		//�G�t�F�N�g�̍X�V
		item->Update();

		//�G�t�F�N�g�̍��W
		MyEngine::Vector3 pos = item->GetPos();

		// �G�t�F�N�g���Đ�����
		if (item->IsEndLoop())
		{
			Effekseer::EffectRef ref = GetEffekseerEffect(m_handles[item->GetPath()]);
			auto manager = GetEffekseer3DManager();
			Effekseer::Vector3D pos;

			pos.X = item->GetPos().x;
			pos.Y = item->GetPos().y;
			pos.Z = item->GetPos().z;
			StopEffekseer3DEffect(item->GetHandle());
			item->SetHandle(manager->Play(ref, pos, item->GetLoopStartFrame()));
			item->ResetLoop();
		}

		//�G�t�F�N�g�̍��W��ݒ�
		SetPosPlayingEffekseer3DEffect(item->GetHandle(), pos.x, pos.y, pos.z);
		//�G�t�F�N�g�̉�]��ݒ�
		MyEngine::Vector3 rot = item->GetRotation();
		SetRotationPlayingEffekseer3DEffect(item->GetHandle(), rot.x, rot.y, rot.z);

		//���C�t�^�C�����I�����Ă���G�t�F�N�g�������
		if (item->IsEndLifeTime())
		{
			deleteEffects.push_back(item);
		}
	}
	for (auto& item : deleteEffects)
	{
		Exit(item);
	}


}

void EffectManager::Draw()
{
	Effekseer_Sync3DSetting();

	DrawEffekseer3D();
}
