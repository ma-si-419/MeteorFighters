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
#include "SoundManager.h"

namespace
{
	//BGMの音量
	constexpr int kBgmVolume = 220;

	//stringからSuccessKindに変換するためのmap
	const std::map<std::string, TutorialManager::TutorialSuccessKind> kSuccessKindMap =
	{
		{"移動",TutorialManager::TutorialSuccessKind::kMove},
		{"ジャンプ",TutorialManager::TutorialSuccessKind::kJump},
		{"ステップ",TutorialManager::TutorialSuccessKind::kStep},
		{"ダッシュ",TutorialManager::TutorialSuccessKind::kDash},
		{"空中待機",TutorialManager::TutorialSuccessKind::kSkyIdle},
		{"上昇",TutorialManager::TutorialSuccessKind::kUp},
		{"下降",TutorialManager::TutorialSuccessKind::kDown},
		{"格闘攻撃1",TutorialManager::TutorialSuccessKind::kPhysicalAttack1},
		{"格闘攻撃2",TutorialManager::TutorialSuccessKind::kPhysicalAttack2},
		{"格闘攻撃3",TutorialManager::TutorialSuccessKind::kPhysicalAttack3},
		{"格闘攻撃4",TutorialManager::TutorialSuccessKind::kPhysicalAttack4},
		{"格闘攻撃5",TutorialManager::TutorialSuccessKind::kPhysicalAttack5},
		{"格闘攻撃6",TutorialManager::TutorialSuccessKind::kPhysicalAttack6},
		{"格闘攻撃7",TutorialManager::TutorialSuccessKind::kPhysicalAttack7},
		{"格闘攻撃8",TutorialManager::TutorialSuccessKind::kPhysicalAttack8},
		{"スマッシュ格闘",TutorialManager::TutorialSuccessKind::kChargePhysicalAttack},
		{"追い打ち",TutorialManager::TutorialSuccessKind::kChaseAttack},
		{"気力チャージ",TutorialManager::TutorialSuccessKind::kEnergyCharge},
		{"気弾攻撃",TutorialManager::TutorialSuccessKind::kEnergyAttack},
		{"スマッシュ気弾",TutorialManager::TutorialSuccessKind::kChargeEnergyAttack},
		{"必殺技",TutorialManager::TutorialSuccessKind::kSpecialAttack},
		{"ガード",TutorialManager::TutorialSuccessKind::kGuard},
		{"ジャストガード",TutorialManager::TutorialSuccessKind::kJustGuard},
		{"受け身",TutorialManager::TutorialSuccessKind::kFalls},
		{"復帰",TutorialManager::TutorialSuccessKind::kReturn},
		{"スーパーダッシュ",TutorialManager::TutorialSuccessKind::kSuperDash},
		{"ロケットダッシュ",TutorialManager::TutorialSuccessKind::kRocketDash},
		{"ラッシュ対決",TutorialManager::TutorialSuccessKind::kButtonBashing},
		{"派生攻撃上",TutorialManager::TutorialSuccessKind::kUpperAttack},
		{"派生攻撃中",TutorialManager::TutorialSuccessKind::kDekaKick},
		{"派生攻撃下",TutorialManager::TutorialSuccessKind::kCycloneKick},
		{"上ガード",TutorialManager::TutorialSuccessKind::kUpGuard},
		{"下ガード",TutorialManager::TutorialSuccessKind::kDownGuard}
	};

	//stringからTutorialKindに変換するためのmap
	const std::map<std::string, TutorialManager::TutorialKind> kTutorialKindMap =
	{
		{"移動",TutorialManager::TutorialKind::kMove},
		{"ステップ",TutorialManager::TutorialKind::kStep},
		{"ダッシュ",TutorialManager::TutorialKind::kDash},
		{"空中移動",TutorialManager::TutorialKind::kSkyMove},
		{"ラッシュ格闘",TutorialManager::TutorialKind::kPhysicalAttack},
		{"スマッシュ格闘",TutorialManager::TutorialKind::kChargePhysicalAttack},
		{"追い打ち",TutorialManager::TutorialKind::kChaseAttack},
		{"気力チャージ",TutorialManager::TutorialKind::kEnergyCharge},
		{"ラッシュ気弾",TutorialManager::TutorialKind::kEnergyAttack},
		{"スマッシュ気弾",TutorialManager::TutorialKind::kChargeEnergyAttack},
		{"ガード",TutorialManager::TutorialKind::kGuard},
		{"必殺技",TutorialManager::TutorialKind::kSpecialAttack},
		{"高速回避",TutorialManager::TutorialKind::kJustGuard},
		{"受け身",TutorialManager::TutorialKind::kFalls},
		{"復帰",TutorialManager::TutorialKind::kReturn},
		{"スーパーダッシュ",TutorialManager::TutorialKind::kSuperDash},
		{"ロケットダッシュ",TutorialManager::TutorialKind::kRocketDash},
		{"ラッシュ対決",TutorialManager::TutorialKind::kButtonBashing},
		{"アッパーアタック",TutorialManager::TutorialKind::kUpperAttack},
		{"デカキック",TutorialManager::TutorialKind::kMiddleAttack},
		{"サイクロンキック",TutorialManager::TutorialKind::kDownAttack},
		{"上ガード",TutorialManager::TutorialKind::kGuardUp},
		{"下ガード",TutorialManager::TutorialKind::kGuardDown}
	};

	//stringからEnemyInput::Actionに変換するためのmap
	const std::map<std::string, EnemyInput::Action> kActionMap =
	{
		{"Idle",EnemyInput::Action::kNone},
		{"EnergyAttack",EnemyInput::Action::kEnergyAttack},
		{"PhysicalAttack",EnemyInput::Action::kPhysicalAttack},
		{"Dash",EnemyInput::Action::kDash},
		{"SuperDash",EnemyInput::Action::kSuperDash},
		{"SpecialAttack",EnemyInput::Action::kSpecialAttack},
		{"EnergyCharge",EnemyInput::Action::kEnergyCharge},
		{"Guard",EnemyInput::Action::kGuard},
		{"MiddleChargeAttack",EnemyInput::Action::kMiddleChargeAttack},
		{"DownChargeAttack",EnemyInput::Action::kDownChargeAttack},
		{"UpChargeAttack",EnemyInput::Action::kUpChargeAttack},
		{"RocketDash",EnemyInput::Action::kRocketDash},
		{"None",EnemyInput::Action::kNone}
	};

	//BGMの名前
	const std::string kBgmName = "Bgm0";
}

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
	ChangeSituation(TutorialSituation::kStartMenu);

	//エネミーのインプットデータを持っておく
	m_pEnemyInput = m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetEnemyInput();
}

void TutorialManager::Update()
{
	if (!SoundManager::GetInstance().IsPlayingSound(kBgmName))
	{
		//BGMを再生する
		SoundManager::GetInstance().PlayLoopSound(kBgmName);

		//BGMの音量を設定する
		SoundManager::GetInstance().SetSoundVolume(kBgmName, kBgmVolume);
	}

	(this->*m_updateSituationFunc)();

	//常にエネミーの入力を更新する
	ChangeEnemyAction(m_tutorialPlayData[m_pTutorialUi->GetTutorialNumber()][static_cast<int>(TutorialPlayDataIndex::kEnemySituation)]);

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

	//BGMを止める
	SoundManager::GetInstance().StopLoopSound(kBgmName);
}

void TutorialManager::UpdateStartMenu()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	auto selectItem = m_pTutorialUi->GetStartMenuSelectItem();

	if (input->IsTrigger("A") && m_pTutorialUi->IsSelectStartMenu())
	{
		//チュートリアルを開始するが押されたら
		if (selectItem == TutorialUi::StartMenuItem::kStartTutorial)
		{
			//サウンドを再生
			SoundManager::GetInstance().PlayOnceSound("Ok");

			ChangeSituation(TutorialSituation::kStart);

			//体力バーを表示する
			m_isDrawHpBar = true;
		}
		//チュートリアルを選択するが押されたら
		else if (selectItem == TutorialUi::StartMenuItem::kSelectTutorial)
		{
			//サウンドを再生
			SoundManager::GetInstance().PlayOnceSound("Ok");

			//チュートリアルを選択する画面に移動
			ChangeSituation(TutorialSituation::kSelectMenu);
		}
		else if (selectItem == TutorialUi::StartMenuItem::kEnd)
		{
			//サウンドを再生
			SoundManager::GetInstance().PlayOnceSound("Cancel");

			//メインメニューに戻る
			m_nextScene = Game::Scene::kMenu;
		}
	}
	//カメラの更新を行う
	m_pCamera->Update();
}

void TutorialManager::UpdatePlayMenu()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	TutorialUi::PlayMenuItem selectItem = m_pTutorialUi->GetPlayMenuSelectItem();

	bool isEnd = false;

	auto nextSituation = TutorialSituation::kPlaying;

	if (input->IsTrigger("A"))
	{
		//サウンドを再生
		SoundManager::GetInstance().PlayOnceSound("Ok");

		//状況をリセットするを押されたら
		if (selectItem == TutorialUi::PlayMenuItem::kReset)
		{
			RetryInit();
			//カメラを初期化
			m_pCamera->SetLocalPos();
			m_nowTutorial = static_cast<TutorialKind>(m_pTutorialUi->GetTutorialNumber());
			isEnd = true;
			nextSituation = TutorialSituation::kStart;

			for (auto& item : m_successTutorialKinds)
			{
				item.second = false;
			}
		}
		//メニューを閉じるが押されたら
		else if (selectItem == TutorialUi::PlayMenuItem::kMenuEnd)
		{
			isEnd = true;
			//今行っているチュートリアルと選択しているチュートリアルが異なっていたら
			if (m_nowTutorial != static_cast<TutorialKind>(m_pTutorialUi->GetTutorialNumber()))
			{
				m_nowTutorial = static_cast<TutorialKind>(m_pTutorialUi->GetTutorialNumber());
				RetryInit();

				for (auto& item : m_successTutorialKinds)
				{
					item.second = false;
				}
			}

			nextSituation = TutorialSituation::kPlaying;
		}
		//チュートリアルセレクト画面に戻るが押されたら
		else if (selectItem == TutorialUi::PlayMenuItem::kSelectMenu)
		{
			RetryInit();
			m_pCamera->SetLocalPos();
			ChangeSituation(TutorialSituation::kSelectMenu);
		}
		//チュートリアルを終了するが押されたら
		else if (selectItem == TutorialUi::PlayMenuItem::kTutorialEnd)
		{
			//メインメニューに戻る
			m_nextScene = Game::Scene::kMenu;
		}
	}
	else if (input->IsTrigger("B"))
	{
		isEnd = true;

		nextSituation = TutorialSituation::kPlaying;

		//今行っているチュートリアルと選択しているチュートリアルが異なっていたら
		if (m_nowTutorial != static_cast<TutorialKind>(m_pTutorialUi->GetTutorialNumber()))
		{
			m_nowTutorial = static_cast<TutorialKind>(m_pTutorialUi->GetTutorialNumber());
			RetryInit();

			nextSituation = TutorialSituation::kStart;

			for (auto& item : m_successTutorialKinds)
			{
				item.second = false;
			}
		}

		//サウンドを再生
		SoundManager::GetInstance().PlayOnceSound("Cancel");

	}

	//メニュー画面を閉じる選択肢が選ばれていたら
	if (isEnd)
	{
		ChangeSituation(nextSituation);

		for (auto& item : m_successTutorialKinds)
		{
			item.second = false;
		}
	}
}

void TutorialManager::UpdateStart()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//クリア判定をリセット
	for (auto& item : m_successTutorialKinds)
	{
		item.second = false;
	}

	if (input->IsTrigger("A"))
	{
		ChangeSituation(TutorialSituation::kPlaying);

		//サウンドを再生
		SoundManager::GetInstance().PlayOnceSound("Ok");
	}

	//カメラの更新を行う
	m_pCamera->Update();

}

void TutorialManager::UpdatePlaying()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	if (input->IsTrigger("Pause"))
	{
		//サウンドを再生
		SoundManager::GetInstance().PlayOnceSound("Ok");

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


	for (auto item : successTerms)
	{
		//ここでクリアしているかを確認
		if (!m_successTutorialKinds[item])
		{
			//一つでもクリアしていない項目があればfalseにする
			isSuccess = false;
		}
	}

	if (m_pTutorialUi->GetNowTutorialMode() == static_cast<int>(TutorialManager::TutorialMode::kStop))
	{
		isSuccess = false;
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
		//モードによって次に進むかどうかを変える
		if (m_pTutorialUi->GetNowTutorialMode() == static_cast<int>(TutorialMode::kAuto))
		{
			//次のチュートリアルに進む
			m_nowTutorial = static_cast<TutorialKind>(static_cast<int>(m_nowTutorial) + 1);
		}
		else if (m_pTutorialUi->GetNowTutorialMode() == static_cast<int>(TutorialMode::kRepeat))
		{
			//同じチュートリアルを繰り返す
		}


		for (auto& item : m_successTutorialKinds)
		{
			item.second = false;
		}

		//全てのチュートリアルをクリアしていたら
		if (m_nowTutorial == TutorialKind::kTutorialEnd)
		{
			ChangeSituation(TutorialSituation::kStartMenu);
		}
		else
		{
			ChangeSituation(TutorialSituation::kStart);
		}
	}

	//カメラの更新を行う
	m_pCamera->Update();
}

void TutorialManager::UpdateSelectMenu()
{
	//入力情報
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//選択できる状況であれば
	if (m_pTutorialUi->IsSelectSelectMenu())
	{
		//決定ボタンが押されたら
		if (input->IsTrigger("A"))
		{
			m_nowTutorial = kTutorialKindMap.at(m_pTutorialUi->GetSelectTutorialName());

			ChangeSituation(TutorialSituation::kStart);

			//サウンドを再生する
			SoundManager::GetInstance().PlayOnceSound("Ok");

		}
		//戻るボタンが押されたら
		else if (input->IsTrigger("B"))
		{
			ChangeSituation(TutorialSituation::kStartMenu);
			//サウンドを再生する
			SoundManager::GetInstance().PlayOnceSound("Cancel");
		}
	}

	//カメラの更新を行う
	m_pCamera->Update();
}

void TutorialManager::DrawStartMenu()
{
	m_pTutorialUi->DrawStartMenu();
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

void TutorialManager::DrawSelectMenu()
{
	m_pTutorialUi->DrawSelectMenu();
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
	//スタートメニュー時
	else if (next == TutorialSituation::kStartMenu)
	{
		//プレイヤーの状況を変更
		for (auto& player : m_pCharacters)
		{
			player->ChangeSituationUpdate(static_cast<int>(BattleSituation::kIdle));
			player->LookTarget();
		}
		//初期化を行う
		m_pTutorialUi->InitStartMenu();
		//更新処理の変更
		m_updateSituationFunc = &TutorialManager::UpdateStartMenu;
		//描画処理の変更
		m_drawSituationFunc = &TutorialManager::DrawStartMenu;
		//HPバーを非表示にする
		m_isDrawHpBar = false;
	}
	//チュートリアルセレクト時
	else if (next == TutorialSituation::kSelectMenu)
	{
		//プレイヤーの状況を変更
		for (auto& player : m_pCharacters) player->ChangeSituationUpdate(static_cast<int>(BattleSituation::kIdle));
		//初期化を行う
		m_pTutorialUi->InitSelectMenu();
		//更新処理の変更
		m_updateSituationFunc = &TutorialManager::UpdateSelectMenu;
		//描画処理の変更
		m_drawSituationFunc = &TutorialManager::DrawSelectMenu;
		//HPバーを非表示にする
		m_isDrawHpBar = false;
	}
	//開始時
	else if (next == TutorialSituation::kStart)
	{
		//プレイヤーの状況を変更
		for (auto& player : m_pCharacters)
		{
			player->ChangeSituationUpdate(static_cast<int>(BattleSituation::kIdle));
			player->RetryInit();
		}
		//初期化を行う
		m_pTutorialUi->InitStart();
		//更新処理の変更
		m_updateSituationFunc = &TutorialManager::UpdateStart;
		//描画処理の変更
		m_drawSituationFunc = &TutorialManager::DrawStart;
		//HPバーを表示する
		m_isDrawHpBar = true;
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
		m_pTutorialUi->SetNowTutorialNumber(static_cast<int>(m_nowTutorial));
		//更新処理の変更
		m_updateSituationFunc = &TutorialManager::UpdatePlaying;
		//描画処理の変更
		m_drawSituationFunc = &TutorialManager::DrawPlaying;
		//HPバーを表示する
		m_isDrawHpBar = true;
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
	return kSuccessKindMap.at(kind);
}

TutorialManager::TutorialKind TutorialManager::ChangeStringToTutorialKind(std::string kind)
{
	return kTutorialKindMap.at(kind);
}

void TutorialManager::ChangeEnemyAction(std::string action)
{
	//エネミーの行動を変更する
	m_pEnemyInput->SetTutorialAction(kActionMap.at(action));
}
