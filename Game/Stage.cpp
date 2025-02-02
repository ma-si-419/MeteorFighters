#include "Stage.h"
#include "Physics.h"
#include "EffectManager.h"
#include "Effect.h"

namespace
{
	const VECTOR kStagePos = VGet(300, 0, 0);

	const VECTOR kSkyDomeScale = VGet(50.0f, 50.0f, 50.0f);

	const VECTOR kStageScale = VGet(0.3f, 0.3f, 0.3f);

	//エフェクトの座標
	const VECTOR kEffectPos = VGet(0, -80, 0);

	constexpr float kSkyDomeRotaSpeed = 0.001f;
}

Stage::Stage(std::shared_ptr<EffectManager> manager) :
	m_skyDomeHandle(-1),
	m_stageHandle(-1)
{
	m_pEffectManager = manager;

	m_stagePath = "data/model/Stage.mv1";
	m_skyDomePath = "data/model/Dome.mv1";
}

Stage::~Stage()
{

}

void Stage::Init()
{
	m_pEffect = std::make_shared<Effect>("StageEdge");

	m_pEffect->SetLoop(1.0f, 240.0f);
	m_pEffect->SetPos(kEffectPos);

	m_pEffect->Init(m_pEffectManager, kEffectPos);
}

void Stage::Update()
{

	auto rota = MV1GetRotationXYZ(m_skyDomeHandle);

	rota.y += kSkyDomeRotaSpeed;

	MV1SetRotationXYZ(m_skyDomeHandle, rota);

}

void Stage::Draw()
{
	MV1DrawModel(m_skyDomeHandle);
	MV1DrawModel(m_stageHandle);
}

void Stage::Final()
{
	m_pEffect->End();

	MV1DeleteModel(m_skyDomeHandle);
	MV1DeleteModel(m_stageHandle);
	Physics::GetInstance().DeleteStage();
}

void Stage::SetStageModelHandle(int handle)
{
	m_stageHandle = handle;

	//ステージのモデル設定
	MV1SetPosition(m_stageHandle, kStagePos);
	MV1SetScale(m_stageHandle, kStageScale);
	MV1SetupCollInfo(m_stageHandle);

	Physics::GetInstance().SetStage(m_stageHandle);
}

void Stage::SetSkyDomeModelHandle(int handle)
{
	m_skyDomeHandle = handle;

	//スカイドームの設定
	MV1SetPosition(m_skyDomeHandle, kStagePos);
	MV1SetScale(m_skyDomeHandle, kSkyDomeScale);
}
