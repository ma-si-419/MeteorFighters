#include "TutorialManager.h"
#include "TutorialUi.h"
#include "GameUi.h"
#include "LoadCsv.h"
#include "GameCamera.h"
#include "Attack.h"
#include "EffectManager.h"
#include "Stage.h"
#include "Input.h"
#include "EnemyInput.h"

namespace
{
	//チュートリアルをクリアする条件
	const std::vector<TutorialManager::TutorialSuccessKind> kTutorialSuccessTerms[static_cast<int>(TutorialManager::TutorialKind::kTutorialNum)] =
	{
		{TutorialManager::TutorialSuccessKind::kMove},
		{TutorialManager::TutorialSuccessKind::kStep},
		{TutorialManager::TutorialSuccessKind::kJump},
		{TutorialManager::TutorialSuccessKind::kUp,TutorialManager::TutorialSuccessKind::kDown },
		{TutorialManager::TutorialSuccessKind::kPhysicalAttack},
		{TutorialManager::TutorialSuccessKind::kChargePhysicalAttack},
		{TutorialManager::TutorialSuccessKind::kEnergyCharge},
		{TutorialManager::TutorialSuccessKind::kEnergyAttack},
		{TutorialManager::TutorialSuccessKind::kChargeEnergyAttack},
		{TutorialManager::TutorialSuccessKind::kGuard},
		{TutorialManager::TutorialSuccessKind::kSpecialAttack}
	};
}

TutorialManager::TutorialManager(std::shared_ptr<GameCamera> camera) :
	GameManagerBase(camera, GameManagerBase::GameKind::kTutorial),
	m_nowTutorial(TutorialKind::kMove),
	m_drawSituationFunc(&TutorialManager::DrawMenu),
	m_updateSituationFunc(&TutorialManager::UpdateMenu),
	m_tutorialSituation(TutorialSituation::kMenu)
{
	m_pTutorialUi = std::make_shared<TutorialUi>();

	m_pGameUi = std::make_shared<GameUi>();
}

TutorialManager::~TutorialManager()
{
}

void TutorialManager::Init()
{
	//カメラの初期化
	m_pCamera->SetBattleCamera();
	
	//チュートリアルUIの初期化
	m_pTutorialUi->Init();

	//situationの変更
	ChangeSituation(TutorialSituation::kMenu);
}

void TutorialManager::Update()
{
	(this->*m_updateSituationFunc)();

	//UIのアップデート
	m_pTutorialUi->Update();


	//カメラの更新
	MyEngine::Vector3 onePlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetPos();
	MyEngine::Vector3 twoPlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetPos();

	MyEngine::Vector3 cameraTargetPos = twoPlayerPos;

	cameraTargetPos.y = twoPlayerPos.y;

	m_pCamera->SetCenterAndTarget(onePlayerPos, cameraTargetPos);

	//1Pから2Pへのベクトル
	MyEngine::Vector3 playerToTarget = (twoPlayerPos - onePlayerPos).Normalize();

	//カメラの正面方向を設定
	m_pCamera->SetFrontPos(onePlayerPos + playerToTarget);

	//カメラにプレイヤーのベロシティを設定する
	m_pCamera->SetPlayerVelo(m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetVelo());

	//ゲームシーン共通の更新
	UpdateCommon();
}

void TutorialManager::Draw()
{
	//共通の描画処理
	DrawCommon();

	(this->*m_drawSituationFunc)();
}

void TutorialManager::Final()
{
	for (auto& character : m_pCharacters)
	{
		character->Final();
	}
	for (auto& attack : m_pAttacks)
	{
		attack->Final();
	}
	m_pStage->Final();
	m_pCamera->Final();
	m_pEffectManager->Final();
}

void TutorialManager::UpdateMenu()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	TutorialUi::MenuItem selectItem = m_pTutorialUi->GetSelectItem();

	if (input->IsTrigger("A"))
	{
		bool isEnd = false;

		//状況をリセットするを押されたら
		if (selectItem == TutorialUi::MenuItem::kReset)
		{
			RetryInit();
			m_nowTutorial = static_cast<TutorialKind>(m_pTutorialUi->GetTutorialNumber());
			isEnd = true;
		}
		//メニューを閉じるが押されたら
		else if (selectItem == TutorialUi::MenuItem::kMenuEnd)
		{
			isEnd = true;
			//今行っているチュートリアルと選択しているチュートリアルが異なっていたら
			if (m_nowTutorial != static_cast<TutorialKind>(m_pTutorialUi->GetTutorialNumber()))
			{
				m_nowTutorial = static_cast<TutorialKind>(m_pTutorialUi->GetTutorialNumber());
				RetryInit();
			}
		}
		//チュートリアルを終了するが押されたら
		else if (selectItem == TutorialUi::MenuItem::kTutorialEnd)
		{
			//メインメニューに戻る
			m_nextScene = Game::Scene::kMenu;
		}

		//メニュー画面を閉じる選択肢が選ばれていたら
		if (isEnd)
		{
			ChangeSituation(TutorialSituation::kStart);
		}
	}
}

void TutorialManager::UpdateStart()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	if (input->IsTrigger("A"))
	{
		ChangeSituation(TutorialSituation::kPlaying);
	}
}

void TutorialManager::UpdatePlaying()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	if (input->IsTrigger("Pause"))
	{
		ChangeSituation(TutorialSituation::kMenu);
	}

	//チュートリアルが成功したかどうか
	bool isSuccess = true;

	auto& terms = kTutorialSuccessTerms[static_cast<int>(m_nowTutorial)];

	for (auto item : terms)
	{
		//ここでクリアしているかを確認
		if (!m_successTutorialKinds[item])
		{
			//一つでもクリアしていない項目があればfalseにする
			isSuccess = false;
		}
	}

	//クリアしたかどうかをUiに送る
	m_pTutorialUi->SetSuccessTutorial(isSuccess);

	//クリアしていたら描画処理を変更する
	if (isSuccess)
	{
		ChangeSituation(TutorialSituation::kSuccess);
	}

}

void TutorialManager::UpdateSuccess()
{
	//クリア演出が終わっていたら次のメニューに移る
	if (m_pTutorialUi->IsSuccessEnd())
	{
		m_nowTutorial = static_cast<TutorialKind>(static_cast<int>(m_nowTutorial) + 1);

		for (auto& item : m_successTutorialKinds)
		{
			item.second = false;
		}

		ChangeSituation(TutorialSituation::kStart);
	}
}

void TutorialManager::DrawMenu()
{
	m_pTutorialUi->DrawMenu();
}

void TutorialManager::DrawStart()
{
	m_pTutorialUi->DrawStart(static_cast<int>(m_nowTutorial));
}

void TutorialManager::DrawPlaying()
{
	m_pTutorialUi->DrawPlaying(static_cast<int>(m_nowTutorial));
}

void TutorialManager::DrawSuccess()
{
	m_pTutorialUi->DrawSuccess(static_cast<int>(m_nowTutorial));
}

void TutorialManager::ChangeSituation(TutorialSituation next)
{
	//状況を変更
	m_tutorialSituation = next;

	//メニュー時
	if (next == TutorialSituation::kMenu)
	{
		//プレイヤーの状況を変更
		for (auto& player : m_pCharacters) player->ChangeSituationUpdate(static_cast<int>(BattleSituation::kMenu));

		//初期化を行う
		m_pTutorialUi->SetNowTutorialNumber(static_cast<int>(m_nowTutorial));
		m_pTutorialUi->InitMenu();
		//更新処理の変更
		m_updateSituationFunc = &TutorialManager::UpdateMenu;
		//描画処理の変更
		m_drawSituationFunc = &TutorialManager::DrawMenu;
	}
	//開始時
	else if (next == TutorialSituation::kStart)
	{
		//プレイヤーの状況を変更
		for (auto& player : m_pCharacters) player->ChangeSituationUpdate(static_cast<int>(BattleSituation::kMenu));

		//初期化を行う
		m_pTutorialUi->InitStart();
		//更新処理の変更
		m_updateSituationFunc = &TutorialManager::UpdateStart;
		//描画処理の変更
		m_drawSituationFunc = &TutorialManager::DrawStart;
	}
	//プレイ中
	else if (next == TutorialSituation::kPlaying)
	{
		//プレイヤーの状況を変更
		for (auto& player : m_pCharacters)
		{
			player->LookTarget();
			player->ChangeSituationUpdate(static_cast<int>(BattleSituation::kBattle));
		}
		//初期化を行う
		m_pTutorialUi->InitPlaying(static_cast<int>(m_nowTutorial));
		//更新処理の変更
		m_updateSituationFunc = &TutorialManager::UpdatePlaying;
		//描画処理の変更
		m_drawSituationFunc = &TutorialManager::DrawPlaying;
	}
	else if (next == TutorialSituation::kSuccess)
	{
		//プレイヤーの状況を変更
		for (auto& player : m_pCharacters) player->ChangeSituationUpdate(static_cast<int>(BattleSituation::kBattle));

		//初期化を行う
		m_pTutorialUi->InitSuccess();
		//更新処理の変更
		m_updateSituationFunc = &TutorialManager::UpdateSuccess;
		//描画処理を変更
		m_drawSituationFunc = &TutorialManager::DrawSuccess;

	}

}
