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

TutorialManager::TutorialManager(std::shared_ptr<GameCamera> camera) :
	GameManagerBase(camera, GameManagerBase::GameKind::kTutorial),
	m_nowTutorial(TutorialKind::kMove),
	m_drawSituationFunc(&TutorialManager::DrawPlayMenu),
	m_updateSituationFunc(&TutorialManager::UpdatePlayMenu),
	m_tutorialSituation(TutorialSituation::kPlayMenu)
{
	m_pTutorialUi = std::make_shared<TutorialUi>();

	m_pGameUi = std::make_shared<GameUi>();

	LoadCsv load;

	m_tutorialPlayData = load.LoadFile("data/csv/tutorialPlayData.csv");

	m_pTutorialUi->SetTutorialPlayData(m_tutorialPlayData);
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
	ChangeSituation(TutorialSituation::kStart);
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

void TutorialManager::UpdateStartMenu()
{
}

void TutorialManager::UpdatePlayMenu()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	TutorialUi::MenuItem selectItem = m_pTutorialUi->GetSelectItem();

	bool isEnd = false;

	if (input->IsTrigger("A"))
	{

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
	}
	else if (input->IsTrigger("B"))
	{
		isEnd = true;
		//今行っているチュートリアルと選択しているチュートリアルが異なっていたら
		if (m_nowTutorial != static_cast<TutorialKind>(m_pTutorialUi->GetTutorialNumber()))
		{
			m_nowTutorial = static_cast<TutorialKind>(m_pTutorialUi->GetTutorialNumber());
			RetryInit();
		}
	}

	//メニュー画面を閉じる選択肢が選ばれていたら
	if (isEnd)
	{
		ChangeSituation(TutorialSituation::kStart);
	}
}

void TutorialManager::UpdateStart()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	if (input->IsTrigger("A"))
	{
		ChangeSituation(TutorialSituation::kPlaying);
	}

	//カメラの更新を行う
	m_pCamera->Update();

}

void TutorialManager::UpdatePlaying()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	if (input->IsTrigger("Pause"))
	{
		ChangeSituation(TutorialSituation::kPlayMenu);
	}

	//チュートリアルが成功したかどうか
	bool isSuccess = true;

	//成功条件
	std::vector<TutorialSuccessKind> successTerms;

	//外部データから成功条件を取得
	for (auto& data : m_tutorialPlayData)
	{
		//チュートリアルの名前が違う場合は次のデータに移る
		if (m_nowTutorial != ChangeStringToTutorialKind(data[static_cast<int>(TutorialPlayDataIndex::kTutorialName)]))continue;

		int loopCount = 0;

		//データの中身を取得
		for (auto item : data)
		{
			//ループ回数が一定数以下だったら
			if (loopCount <= static_cast<int>(TutorialManager::TutorialPlayDataIndex::kButton))
			{
				loopCount++;
				continue;
			}

			if (item == "")continue;

			//成功条件を取得
			successTerms.push_back(ChangeStringToSuccessKind(item));
		}
	}

#ifdef _DEBUG

	for (auto item : successTerms)
	{
		printfDx("%d,", static_cast<int>(item));
	}
	printfDx("\n");

#endif // _DEBUG
	for (auto item : successTerms)
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

	//カメラの更新を行う
	m_pCamera->Update();

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

	//カメラの更新を行う
	m_pCamera->Update();
}

void TutorialManager::DrawStartMenu()
{
}

void TutorialManager::DrawPlayMenu()
{
	m_pTutorialUi->DrawPlayMenu();
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
	if (next == TutorialSituation::kPlayMenu)
	{
		//プレイヤーの状況を変更
		for (auto& player : m_pCharacters) player->ChangeSituationUpdate(static_cast<int>(BattleSituation::kMenu));

		//初期化を行う
		m_pTutorialUi->SetNowTutorialNumber(static_cast<int>(m_nowTutorial));
		m_pTutorialUi->InitPlayMenu();
		//更新処理の変更
		m_updateSituationFunc = &TutorialManager::UpdatePlayMenu;
		//描画処理の変更
		m_drawSituationFunc = &TutorialManager::DrawPlayMenu;
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

TutorialManager::TutorialSuccessKind TutorialManager::ChangeStringToSuccessKind(std::string kind)
{
	if (kind == "移動")
	{
		return TutorialSuccessKind::kMove;
	}
	else if (kind == "ステップ")
	{
		return TutorialSuccessKind::kStep;
	}
	else if (kind == "ダッシュ")
	{
		return TutorialSuccessKind::kDash;
	}
	else if (kind == "空中待機")
	{
		return TutorialSuccessKind::kSkyIdle;
	}
	else if (kind == "上昇")
	{
		return TutorialSuccessKind::kUp;
	}
	else if (kind == "下降")
	{
		return TutorialSuccessKind::kDown;
	}
	else if (kind == "ジャンプ")
	{
		return TutorialSuccessKind::kJump;
	}
	else if (kind == "格闘攻撃1")
	{
		return TutorialSuccessKind::kPhysicalAttack1;
	}
	else if (kind == "格闘攻撃2")
	{
		return TutorialSuccessKind::kPhysicalAttack2;
	}
	else if (kind == "格闘攻撃3")
	{
		return TutorialSuccessKind::kPhysicalAttack3;
	}
	else if (kind == "格闘攻撃4")
	{
		return TutorialSuccessKind::kPhysicalAttack4;
	}
	else if (kind == "格闘攻撃5")
	{
		return TutorialSuccessKind::kPhysicalAttack5;
	}
	else if (kind == "格闘攻撃6")
	{
		return TutorialSuccessKind::kPhysicalAttack6;
	}
	else if (kind == "格闘攻撃7")
	{
		return TutorialSuccessKind::kPhysicalAttack7;
	}
	else if (kind == "格闘攻撃8")
	{
		return TutorialSuccessKind::kPhysicalAttack8;
	}
	else if (kind == "スマッシュ格闘")
	{
		return TutorialSuccessKind::kChargePhysicalAttack;
	}
	else if (kind == "追い打ち")
	{
		return TutorialSuccessKind::kChaseAttack;
	}
	else if (kind == "気力チャージ")
	{
		return TutorialSuccessKind::kEnergyCharge;
	}
	else if (kind == "気弾攻撃")
	{
		return TutorialSuccessKind::kEnergyAttack;
	}
	else if (kind == "スマッシュ気弾")
	{
		return TutorialSuccessKind::kChargeEnergyAttack;
	}
	else if (kind == "必殺技")
	{
		return TutorialSuccessKind::kSpecialAttack;
	}
	else if (kind == "ガード")
	{
		return TutorialSuccessKind::kGuard;
	}
	else if (kind == "ジャストガード")
	{
		return TutorialSuccessKind::kJustGuard;
	}
	else if (kind == "受け身")
	{
		return TutorialSuccessKind::kFalls;
	}
	else if (kind == "復帰")
	{
		return TutorialSuccessKind::kReturn;
	}
	else if (kind == "スーパーダッシュ")
	{
		return TutorialSuccessKind::kSuperDash;
	}
	else if (kind == "ロケットダッシュ")
	{
		return TutorialSuccessKind::kRocketDash;
	}
	else if (kind == "ラッシュ対決")
	{
		return TutorialSuccessKind::kButtonBashing;
	}
	else if (kind == "派生攻撃上")
	{
		return TutorialSuccessKind::kUpperAttack;
	}
	else if (kind == "派生攻撃中")
	{
		return TutorialSuccessKind::kDekaKick;
	}
	else if (kind == "派生攻撃下")
	{
		return TutorialSuccessKind::kCycloneKick;
	}
	else if (kind == "上ガード")
	{
		return TutorialSuccessKind::kUpGuard;
	}
	else if (kind == "下ガード")
	{
		return TutorialSuccessKind::kDownGuard;
	}
	else
	{
		//ここまで来ないはず
		return TutorialSuccessKind::kMove;
	}
}

TutorialManager::TutorialKind TutorialManager::ChangeStringToTutorialKind(std::string kind)
{
	if (kind == "移動")
	{
		return TutorialKind::kMove;
	}
	else if (kind == "ステップ")
	{
		return TutorialKind::kStep;
	}
	else if (kind == "ダッシュ")
	{
		return TutorialKind::kDash;
	}
	else if (kind == "空中移動")
	{
		return TutorialKind::kSkyMove;
	}
	else if (kind == "ラッシュ格闘")
	{
		return TutorialKind::kPhysicalAttack;
	}
	else if (kind == "スマッシュ格闘")
	{
		return TutorialKind::kChargePhysicalAttack;
	}
	else if (kind == "追い打ち")
	{
		return TutorialKind::kChaseAttack;
	}
	else if (kind == "気力チャージ")
	{
		return TutorialKind::kEnergyCharge;
	}
	else if (kind == "ラッシュ気弾")
	{
		return TutorialKind::kEnergyAttack;
	}
	else if (kind == "スマッシュ気弾")
	{
		return TutorialKind::kChargeEnergyAttack;
	}
	else if (kind == "ガード")
	{
		return TutorialKind::kGuard;
	}
	else if (kind == "必殺技")
	{
		return TutorialKind::kSpecialAttack;
	}
	else if (kind == "高速回避")
	{
		return TutorialKind::kJustGuard;
	}
	else if (kind == "受け身")
	{
		return TutorialKind::kFalls;
	}
	else if (kind == "復帰")
	{
		return TutorialKind::kReturn;
	}
	else if (kind == "スーパーダッシュ")
	{
		return TutorialKind::kSuperDash;
	}
	else if (kind == "ロケットダッシュ")
	{
		return TutorialKind::kRocketDash;
	}
	else if (kind == "ラッシュ対決")
	{
		return TutorialKind::kButtonBasging;
	}
	else if (kind == "アッパーアタック")
	{
		return TutorialKind::kUpperAttack;
	}
	else if (kind == "デカキック")
	{
		return TutorialKind::kMiddleAttack;
	}
	else if (kind == "サイクロンキック")
	{
		return TutorialKind::kDownAttack;
	}
	else if (kind == "上ガード")
	{
		return TutorialKind::kGuardUp;
	}
	else if (kind == "下ガード")
	{
		return TutorialKind::kGuardDown;
	}
	else
	{
		//ここまで来ないはず
		return TutorialKind::kMove;
	}

	return TutorialKind();
}
