#include "TitleUi.h"
#include "DxLib.h"
#include <string>
#include "GraphManager.h"
#include "SoundManager.h"
#include "Game.h"

namespace
{
	//フォントの名前
	const TCHAR* kFontName = "GN-キルゴUかなNB";

	//文字の大きさ
	constexpr int kFontSize = 50;

	//文字の色(グレー)
	const unsigned int kFontColor = GetColor(192, 192, 192);
	 
	//カメラのNearFar
	constexpr float kCameraNear = 0.1f;
	constexpr float kCameraFar = 400.0f;

	//スカイドームの拡大率
	constexpr float kSkyDomeScale = 3.0f;

	//スカイドームの回転速度
	constexpr float kSkyDomeRotaSpeed = 0.001f;

	//PRESSANYBUTTONを表示する座標
	constexpr int kStringPosX = 56;
	constexpr int kStringPosY = 710;

	//表示する文字列
	//const std::string kUiString = "PRESS \n    ANY BUTTON";
	const std::string kUiString = "PRESS ANY BUTTON";

	//文字がついている時間
	constexpr int kStringExistTime = 90;

	//文字がついていない時間
	constexpr int kStringNotExistTime = 15;

	//キャラクターを描画する座標
	const MyEngine::Vector2 kCharacterDrawPos[static_cast<int>(TitleUi::DrawCharacterKind::kCharacterKindNum)] =
	{
		{1100,0},
		{Game::kWindowWidth,350},
		{0,600},
		{500,Game::kWindowHeight}
	};

	//キャラクターの初期サイド
	const TitleUi::DrawCharacterSide kCharacterSide[static_cast<int>(TitleUi::DrawCharacterKind::kCharacterKindNum)] =
	{
		TitleUi::DrawCharacterSide::kUp,
		TitleUi::DrawCharacterSide::kRight,
		TitleUi::DrawCharacterSide::kLeft,
		TitleUi::DrawCharacterSide::kDown
	};


	//サイドごとの回転度
	const double kSideRota[4] =
	{
		DX_PI,//180
		DX_PI + DX_PI / 2 ,//270
		0,//0
		DX_PI / 2//90
	};

	//最大何フレームキャラクターが頭を出すか
	constexpr int kMaxCharacterMoveFrame = 60;

	//最低何フレームキャラクターが頭を出すか
	constexpr int kMinCharacterMoveFrame = 30;

	//最初何フレームキャラクターが頭を出すか
	constexpr int kInitCharacterMoveFrame = 120;

	//キャラクターの移動速度
	constexpr int kCharacterMoveSpeed = 20;

	//落下速度の最大値
	constexpr int kMaxFallSpeed = 20;
	//落下速度の最小値
	constexpr int kMinFallSpeed = 10;

	//落下時の回転速度の最大値
	constexpr double kMaxFallRotaSpeed = 0.3;
	//落下時の回転速度の最小値
	constexpr double kMinFallRotaSpeed = 0.05;

	//ジャンプ時の移動速度の最大値
	constexpr int kMaxJumpSpeed = 40;
	//ジャンプ時の移動速度の最小値
	constexpr int kMinJumpSpeed = 30;

	//ジャンプ時にかける重力
	constexpr float kJumpGravity = 0.65f;

	//横っ飛び時の横移動速度の最大値
	constexpr int kMaxSideJumpSpeed = 30;
	//横っ飛び時の横移動速度の最小値
	constexpr int kMinSideJumpSpeed = 20;
	//横っ飛び時の初期上昇速度の最大値
	constexpr int kMaxSideJumpUpSpeed = 15;
	//横っ飛び時の初期上昇速度の最小値
	constexpr int kMinSideJumpUpSpeed = 5;
	//横っ飛び時にかける重力
	constexpr float kSideJumpGravity = 0.3f;

	//後ろを飛んでいくキャラクターの移動速度
	constexpr int kBackCharacterMoveSpeed = 10;
	//後ろを飛んでいくキャラクターのY座標(画面中央)
	constexpr int kBackCharacterPosY = Game::kWindowHeight / 2;
	//後ろを飛んでいくキャラクターの拡大率
	constexpr double kBackCharacterScale = 0.3;
	//後ろを飛んでいくキャラクターの移動速度の最大値
	constexpr int kMaxBackCharacterMoveSpeed = 15;
	//後ろを飛んでいくキャラクターの移動速度の最小値
	constexpr int kMinBackCharacterMoveSpeed = 5;
	//後ろを飛んでいくキャラクターの回転速度の最大値
	constexpr double kMaxBackCharacterRotaSpeed = 0.35;
	//後ろを飛んでいくキャラクターの回転速度の最小値
	constexpr double kMinBackCharacterRotaSpeed = 0.15;


	//キャラクターの画像の幅
	constexpr int kCharacterWidth = 350;
	constexpr int kCharacterHeight = 200;

	//キャラクターのハンドルの種類
	constexpr int kCharacterHandleKind = 1;

	//アップデートの種類の数
	constexpr int kUpdateKindNum = 5;

	//アップデートそれぞれの確率
	constexpr int kUpdateProb[kUpdateKindNum] =
	{
		100,//通常
		10,//落ちる
		20,//横っ飛び
		20,//後ろを飛んでいく
		16 //ジャンプ
	};

	//確率をすべて足した値
	constexpr int kUpdateProbTotal = kUpdateProb[0] + kUpdateProb[1] + kUpdateProb[2] + kUpdateProb[3] + kUpdateProb[4];
}

TitleUi::TitleUi() :
	m_skyDomeHandle(-1),
	m_time(0),
	m_isExistString(true),
	m_fontHandle(-1)
{
	//フォントのハンドルの取得
	m_fontHandle = CreateFontToHandle(kFontName, kFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 3);
}

TitleUi::~TitleUi()
{
	DeleteFontToHandle(m_fontHandle);
}

void TitleUi::Init()
{
	//スカイドームのモデルロード
	m_skyDomeHandle = MV1LoadModel("data/model/Dome.mv1");

	//カメラの設定
	SetCameraPositionAndTarget_UpVecY(VGet(0, 0, 0), VGet(0, 0, 1));

	//NearFarの設定
	SetCameraNearFar(kCameraNear, kCameraFar);

	//スカイドームの設定
	MV1SetPosition(m_skyDomeHandle, VGet(0, 0, 0));

	//スカイドームの拡大率設定
	MV1SetScale(m_skyDomeHandle, VGet(kSkyDomeScale, kSkyDomeScale, kSkyDomeScale));

	//キャラクターの初期設定
	for (auto& item : m_characterDrawInfo)
	{
		//キャラクターの種類設定
		item.kind = static_cast<DrawCharacterKind>(&item - &m_characterDrawInfo[0]);

		//キャラクターの初期座標設定
		item.pos = kCharacterDrawPos[static_cast<int>(item.kind)];

		//キャラクターの初期回転速度設定
		item.rotaSpeed = 0.0;

		//キャラクターの初期移動時間設定
		item.moveTime = kInitCharacterMoveFrame;

		//キャラクターの初期サイド設定
		item.side = kCharacterSide[static_cast<int>(item.kind)];

		//キャラクターの初期回転度設定
		item.rota = kSideRota[static_cast<int>(item.side)];

		//キャラクターの初期拡大率設定
		item.scale = 1.0;

		//キャラクターの初期ハンドル設定
		item.handle = GetCharacterHandle(item.kind, static_cast<CharacterGraphKind>(GetRand(kCharacterHandleKind)));

		//キャラクターの初期更新関数設定
		item.updateFunc = &TitleUi::NormalUpdate;
	}
}

void TitleUi::Update()
{
	//文字列の時間計測関係
	m_time++;

	//文字列を表示していた場合
	if (m_isExistString)
	{
		//一定時間経過していたら文字列を消す
		if (m_time > kStringExistTime)
		{
			//時間をリセット
			m_time = 0;

			m_isExistString = false;
		}
	}
	//文字列を表示していなかった場合
	else
	{
		//一定時間経過していたら文字列を表示する
		if (m_time > kStringNotExistTime)
		{
			//時間をリセット
			m_time = 0;

			m_isExistString = true;
		}
	}

	//スカイドームを回転させる
	MV1SetRotationXYZ(m_skyDomeHandle, VGet(0, MV1GetRotationXYZ(m_skyDomeHandle).y + kSkyDomeRotaSpeed, 0));

	//キャラクターの更新を行う
	for (auto& item : m_characterDrawInfo)
	{
		//キャラクターの更新関数を呼ぶ
		(this->*item.updateFunc)(static_cast<int>(item.kind));
	}
}

void TitleUi::Draw()
{
	auto& manager = GraphManager::GetInstance();

	//スカイドームの描画
	MV1DrawModel(m_skyDomeHandle);

	//後ろを飛んでいくキャラクターの描画
	for (auto& item : m_characterDrawInfo)
	{
		if (item.updateFunc == &TitleUi::BackFryUpdate)
		{
			DrawRotaGraph(static_cast<int>(item.pos.x), static_cast<int>(item.pos.y), item.scale, item.rota, item.handle, true);
		}
	}

	//タイトルロゴの描画
	DrawGraph(0, 0, manager.GetHandle("TitleLogo"), true);

	//後ろを飛んでいくキャラクター以外の描画
	for (auto& item : m_characterDrawInfo)
	{
		if (item.updateFunc != &TitleUi::BackFryUpdate)
		{
			DrawRotaGraph(static_cast<int>(item.pos.x), static_cast<int>(item.pos.y), 1.0, item.rota, item.handle, true);
		}
	}

	//文字列の描画
	if (m_isExistString)
	{
		DrawStringToHandle(kStringPosX, kStringPosY, kUiString.c_str(), kFontColor, m_fontHandle, GetColor(0, 0, 0));
	}


}


MyEngine::Vector2 TitleUi::GetEnterMoveVec(DrawCharacterSide side)
{
	if (side == DrawCharacterSide::kUp)
	{
		return MyEngine::Vector2(0, kCharacterMoveSpeed);
	}
	else if (side == DrawCharacterSide::kRight)
	{
		return MyEngine::Vector2(-kCharacterMoveSpeed, 0);
	}
	else if (side == DrawCharacterSide::kLeft)
	{
		return MyEngine::Vector2(kCharacterMoveSpeed, 0);
	}
	else if (side == DrawCharacterSide::kDown)
	{
		return MyEngine::Vector2(0, -kCharacterMoveSpeed);
	}

	return MyEngine::Vector2(0, 0);
}

MyEngine::Vector2 TitleUi::GetReturnMoveVec(DrawCharacterSide side)
{
	if (side == DrawCharacterSide::kUp)
	{
		return MyEngine::Vector2(0, -kCharacterMoveSpeed);
	}
	else if (side == DrawCharacterSide::kRight)
	{
		return MyEngine::Vector2(kCharacterMoveSpeed, 0);
	}
	else if (side == DrawCharacterSide::kLeft)
	{
		return MyEngine::Vector2(-kCharacterMoveSpeed, 0);
	}
	else if (side == DrawCharacterSide::kDown)
	{
		return MyEngine::Vector2(0, kCharacterMoveSpeed);
	}

	return MyEngine::Vector2(0, 0);
}

MyEngine::Vector2 TitleUi::ClampPos(DrawCharacterSide side, MyEngine::Vector2 pos)
{
	MyEngine::Vector2 ans = pos;

	if (side == DrawCharacterSide::kUp)
	{
		ans.y = min(pos.y, kCharacterHeight / 2);
	}
	else if (side == DrawCharacterSide::kRight)
	{
		ans.x = max(pos.x, Game::kWindowWidth - kCharacterHeight / 2);
	}
	else if (side == DrawCharacterSide::kLeft)
	{
		ans.x = min(pos.x, kCharacterHeight / 2);
	}
	else if (side == DrawCharacterSide::kDown)
	{
		ans.y = max(pos.y, Game::kWindowHeight - kCharacterHeight / 2);
	}

	return ans;
}

MyEngine::Vector2 TitleUi::SetRandomPos(DrawCharacterSide side)
{
	//上方向から出る場合
	if (side == DrawCharacterSide::kUp)
	{
		int randomX = GetRand(Game::kWindowWidth - kCharacterWidth) + kCharacterWidth / 2;
		return MyEngine::Vector2(randomX, -kCharacterWidth);
	}
	//右方向から出る場合
	else if (side == DrawCharacterSide::kRight)
	{
		int randomY = GetRand(Game::kWindowHeight - kCharacterWidth) + kCharacterWidth / 2;
		return MyEngine::Vector2(Game::kWindowWidth + kCharacterWidth, randomY);
	}
	//左方向から出る場合
	else if (side == DrawCharacterSide::kLeft)
	{
		int randomY = GetRand(Game::kWindowHeight - kCharacterWidth) + kCharacterWidth / 2;
		return MyEngine::Vector2(-kCharacterWidth, randomY);
	}
	//下方向から出る場合
	else if (side == DrawCharacterSide::kDown)
	{
		int randomX = GetRand(Game::kWindowWidth - kCharacterWidth) + kCharacterWidth / 2;
		return MyEngine::Vector2(randomX, Game::kWindowHeight + kCharacterWidth);
	}

	return MyEngine::Vector2(0, 0);
}

bool TitleUi::CheckPos(MyEngine::Vector2 pos)
{

	if (pos.x >= -kCharacterHeight &&
		pos.x <= Game::kWindowWidth + kCharacterHeight &&
		pos.y >= -kCharacterHeight &&
		pos.y <= Game::kWindowHeight + kCharacterHeight)
	{
		return true;
	}
	return false;
}

int TitleUi::GetCharacterHandle(DrawCharacterKind characterKind, CharacterGraphKind graphKind)
{
	//返すハンドル
	int ans = -1;

	//グラフィックマネージャーの取得
	auto& manager = GraphManager::GetInstance();

	//ハンドル名
	std::string handleName;

	if (characterKind == DrawCharacterKind::kMouse)
	{
		handleName = "Mouse1P";
	}
	else if (characterKind == DrawCharacterKind::kBlue)
	{
		handleName = "Blue1P";
	}
	else if (characterKind == DrawCharacterKind::kMouse2P)
	{
		handleName = "Mouse2P";
	}
	else if (characterKind == DrawCharacterKind::kBlue2P)
	{
		handleName = "Blue2P";
	}

	if (graphKind == CharacterGraphKind::kChoki)
	{
		handleName += "Choki";
	}
	else if (graphKind == CharacterGraphKind::kPar)
	{
		handleName += "Par";
	}

	ans = manager.GetHandle(handleName);

	if (ans == -1)
	{
		printfDx("ハンドルが取得できませんでした\n");
	}

	return ans;
}

void TitleUi::SetCharacterUpdateFunc(CharacterUpdateKind kind, DrawCharacterKind character)
{

	//通常の更新
	if (kind == CharacterUpdateKind::kNormal)
	{
		InitNormal(static_cast<int>(character));
		m_characterDrawInfo[static_cast<int>(character)].updateFunc = &TitleUi::NormalUpdate;
	}
	//落下の更新
	else if (kind == CharacterUpdateKind::kFall)
	{
		InitFall(static_cast<int>(character));
		m_characterDrawInfo[static_cast<int>(character)].updateFunc = &TitleUi::FallUpdate;
	}
	//ジャンプの更新
	else if (kind == CharacterUpdateKind::kJump)
	{
		InitJump(static_cast<int>(character));
		m_characterDrawInfo[static_cast<int>(character)].updateFunc = &TitleUi::JumpUpdate;
	}
	//横っ飛びの更新
	else if (kind == CharacterUpdateKind::kSideJump)
	{
		InitSideJump(static_cast<int>(character));
		m_characterDrawInfo[static_cast<int>(character)].updateFunc = &TitleUi::SideJumpUpdate;
	}
	//後ろを飛んでいくキャラクターの更新
	else if (kind == CharacterUpdateKind::kBackFry)
	{
		InitBackFry(static_cast<int>(character));
		m_characterDrawInfo[static_cast<int>(character)].updateFunc = &TitleUi::BackFryUpdate;
	}
}

void TitleUi::InitNormal(int number)
{
	//サイドをランダムで設定
	m_characterDrawInfo[number].side = static_cast<DrawCharacterSide>(GetRand(static_cast<int>(DrawCharacterSide::kSideNum)));

	//座標をランダムで設定
	m_characterDrawInfo[number].pos = SetRandomPos(m_characterDrawInfo[number].side);

	//移動時間をランダムで設定
	m_characterDrawInfo[number].moveTime = GetRand(kMaxCharacterMoveFrame) + kMinCharacterMoveFrame;

	//ハンドルをランダムで設定
	m_characterDrawInfo[number].handle = GetCharacterHandle(m_characterDrawInfo[number].kind, static_cast<CharacterGraphKind>(GetRand(kCharacterHandleKind)));

	//回転度を設定
	m_characterDrawInfo[number].rota = kSideRota[static_cast<int>(m_characterDrawInfo[number].side)];

	//回転速度を設定
	m_characterDrawInfo[number].rotaSpeed = 0.0;

	//拡大率を設定
	m_characterDrawInfo[number].scale = kBackCharacterScale;

}

void TitleUi::InitFall(int number)
{
	//落ちるサウンドを再生
	SoundManager::GetInstance().PlayOnceSound("Fall");
	//移動時間を設定
	m_characterDrawInfo[number].moveTime = kMinCharacterMoveFrame;

	//サイドを上に設定
	m_characterDrawInfo[number].side = DrawCharacterSide::kUp;

	//座標をランダムで設定
	m_characterDrawInfo[number].pos = SetRandomPos(m_characterDrawInfo[number].side);

	//ハンドルをランダムで設定
	m_characterDrawInfo[number].handle = GetCharacterHandle(m_characterDrawInfo[number].kind, static_cast<CharacterGraphKind>(GetRand(kCharacterHandleKind)));

	//移動ベクトルを設定
	m_characterDrawInfo[number].moveVec = MyEngine::Vector2(0, GetRand(kMaxFallSpeed - kMinFallSpeed) + kMinFallSpeed);

	//拡大率を設定
	m_characterDrawInfo[number].scale = kBackCharacterScale;

	//回転速度を設定
	float randomRota = static_cast<float>(GetRand(static_cast<int>((kMaxFallRotaSpeed - kMinFallRotaSpeed) * 100)));

	m_characterDrawInfo[number].rotaSpeed = (randomRota / 100) + kMinFallRotaSpeed;
}

void TitleUi::InitJump(int number)
{
	//落ちるサウンドを再生
	SoundManager::GetInstance().PlayOnceSound("Jump");
	//移動時間を設定
	m_characterDrawInfo[number].moveTime = kMinCharacterMoveFrame;
	//サイドを下に設定
	m_characterDrawInfo[number].side = DrawCharacterSide::kDown;
	//座標をランダムで設定
	m_characterDrawInfo[number].pos = SetRandomPos(m_characterDrawInfo[number].side);
	//ハンドルをランダムで設定
	m_characterDrawInfo[number].handle = GetCharacterHandle(m_characterDrawInfo[number].kind, static_cast<CharacterGraphKind>(GetRand(kCharacterHandleKind)));
	//移動ベクトルを設定
	m_characterDrawInfo[number].moveVec = MyEngine::Vector2(0, -(GetRand(kMaxJumpSpeed - kMinJumpSpeed) + kMinJumpSpeed));
	//回転速度を設定
	m_characterDrawInfo[number].rotaSpeed = 0.0;
	//回転度を変更
	m_characterDrawInfo[number].rota = 0.0;
	//拡大率を設定
	m_characterDrawInfo[number].scale = kBackCharacterScale;
}

void TitleUi::InitSideJump(int number)
{
	//横っ飛びのサウンドを再生
	SoundManager::GetInstance().PlayOnceSound("SideJump");

	//移動時間を設定
	m_characterDrawInfo[number].moveTime = kMinCharacterMoveFrame;

	//サイドを左右どちらかで設定
	int random = GetRand(1);

	//左右どちらか出るかをランダムで設定
	if (random == 0)
	{
		//サイドを設定
		m_characterDrawInfo[number].side = DrawCharacterSide::kLeft;
		//初期移動ベクトルを設定
		m_characterDrawInfo[number].moveVec = MyEngine::Vector2(GetRand(kMaxSideJumpSpeed - kMinSideJumpSpeed) + kMinSideJumpSpeed, -GetRand(kMaxSideJumpUpSpeed - kMinSideJumpUpSpeed) - kMinSideJumpUpSpeed);
		//回転度を設定
		m_characterDrawInfo[number].rota = kSideRota[static_cast<int>(DrawCharacterSide::kLeft)];
	}
	else
	{
		//サイドを設定
		m_characterDrawInfo[number].side = DrawCharacterSide::kRight;
		//初期移動ベクトルを設定
		m_characterDrawInfo[number].moveVec = MyEngine::Vector2(-(GetRand(kMaxSideJumpSpeed - kMinSideJumpSpeed) + kMinSideJumpSpeed), -GetRand(kMaxSideJumpUpSpeed - kMinSideJumpUpSpeed) - kMinSideJumpUpSpeed);
		//回転度を設定
		m_characterDrawInfo[number].rota = kSideRota[static_cast<int>(DrawCharacterSide::kRight)];
	}

	//座標をランダムで設定
	m_characterDrawInfo[number].pos = SetRandomPos(m_characterDrawInfo[number].side);

	//ハンドルをランダムで設定
	m_characterDrawInfo[number].handle = GetCharacterHandle(m_characterDrawInfo[number].kind, static_cast<CharacterGraphKind>(GetRand(kCharacterHandleKind)));

	//拡大率を設定
	m_characterDrawInfo[number].scale = kBackCharacterScale;
}

void TitleUi::InitBackFry(int number)
{
	//移動時間を設定
	m_characterDrawInfo[number].moveTime = kMinCharacterMoveFrame;
	//サイドを右側に設定
	m_characterDrawInfo[number].side = static_cast<DrawCharacterSide>(GetRand(static_cast<int>(DrawCharacterSide::kSideNum)));
	//座標をランダムで設定
	m_characterDrawInfo[number].pos = MyEngine::Vector2(Game::kWindowWidth + kCharacterWidth, kBackCharacterPosY);
	//ハンドルをランダムで設定
	m_characterDrawInfo[number].handle = GetCharacterHandle(m_characterDrawInfo[number].kind, static_cast<CharacterGraphKind>(GetRand(kCharacterHandleKind)));
	//移動ベクトルを設定
	m_characterDrawInfo[number].moveVec = MyEngine::Vector2(-(GetRand(kMaxBackCharacterMoveSpeed - kMinBackCharacterMoveSpeed) + kMinBackCharacterMoveSpeed), 0);
	//回転速度を設定
	m_characterDrawInfo[number].rotaSpeed = -static_cast<float>(GetRand(static_cast<int>((kMaxBackCharacterRotaSpeed - kMinBackCharacterRotaSpeed) * 100)) / 100 + kMinBackCharacterRotaSpeed);
	//回転度を設定
	m_characterDrawInfo[number].rota = kSideRota[static_cast<int>(m_characterDrawInfo[number].side)];
	//拡大率を設定
	m_characterDrawInfo[number].scale = kBackCharacterScale;
}

void TitleUi::NormalUpdate(int number)
{
	m_characterDrawInfo[number].moveTime--;

	//移動する時間になったら
	if (m_characterDrawInfo[number].moveTime < 0)
	{
		//外側に向かう移動ベクトルを取得
		MyEngine::Vector2 moveVec = GetReturnMoveVec(m_characterDrawInfo[number].side);

		//座標を移動
		m_characterDrawInfo[number].pos += moveVec;

		//画面外に出たかどうかをチェック

		//出ていたら座標をランダムで画面外に設定する
		if (!CheckPos(m_characterDrawInfo[number].pos))
		{

			//更新関数をランダムで設定
			int random = 0;

			int prob = GetRand(kUpdateProbTotal);

			//ランダム処理
			for (auto item : kUpdateProb)
			{
				prob -= item;
				if (prob <= 0)
				{
					//ここで決定
					SetCharacterUpdateFunc(static_cast<CharacterUpdateKind>(random), m_characterDrawInfo[number].kind);
					break;
				}
				random++;
			}
		}
	}
	//それ以外
	else
	{
		//内側に向かう移動ベクトルを取得
		MyEngine::Vector2 moveVec = GetEnterMoveVec(m_characterDrawInfo[number].side);

		//座標を移動
		m_characterDrawInfo[number].pos += moveVec;
	}

	m_characterDrawInfo[number].pos = ClampPos(m_characterDrawInfo[number].side, m_characterDrawInfo[number].pos);
}

void TitleUi::FallUpdate(int number)
{
	m_characterDrawInfo[number].moveTime--;

	//一定時間が画面外の判定をしない
	if (m_characterDrawInfo[number].moveTime < 0)
	{
		//画面外に出たら
		if (!CheckPos(m_characterDrawInfo[number].pos))
		{
			//更新関数をランダムで設定
			//更新関数をランダムで設定
			int random = 0;

			int prob = GetRand(kUpdateProbTotal);

			//ランダム処理
			for (auto item : kUpdateProb)
			{
				prob -= item;
				if (prob <= 0)
				{
					//ここで決定
					SetCharacterUpdateFunc(static_cast<CharacterUpdateKind>(random), m_characterDrawInfo[number].kind);
					break;
				}
				random++;
			}
		}
	}
	//座標を移動
	m_characterDrawInfo[number].pos += m_characterDrawInfo[number].moveVec;
	//回転度を変更
	m_characterDrawInfo[number].rota += m_characterDrawInfo[number].rotaSpeed;

}

void TitleUi::JumpUpdate(int number)
{
	m_characterDrawInfo[number].moveTime--;
	//一定時間が画面外の判定をしない
	if (m_characterDrawInfo[number].moveTime < 0)
	{
		//画面外に出たら
		if (!CheckPos(m_characterDrawInfo[number].pos))
		{
			//更新関数をランダムで設定
			int random = 0;

			int prob = GetRand(kUpdateProbTotal);

			//ランダム処理
			for (auto item : kUpdateProb)
			{
				prob -= item;
				if (prob <= 0)
				{
					//ここで決定
					SetCharacterUpdateFunc(static_cast<CharacterUpdateKind>(random), m_characterDrawInfo[number].kind);
					break;
				}
				random++;
			}

		}
	}
	//座標を移動
	m_characterDrawInfo[number].pos += m_characterDrawInfo[number].moveVec;
	//重力をかける
	m_characterDrawInfo[number].moveVec.y += kJumpGravity;
}

void TitleUi::SideJumpUpdate(int number)
{
	m_characterDrawInfo[number].moveTime--;
	//一定時間が画面外の判定をしない
	if (m_characterDrawInfo[number].moveTime < 0)
	{
		//画面外に出たら
		if (!CheckPos(m_characterDrawInfo[number].pos))
		{
			//更新関数をランダムで設定
			int random = 0;

			int prob = GetRand(kUpdateProbTotal);


			//ランダム処理
			for (auto item : kUpdateProb)
			{
				prob -= item;
				if (prob <= 0)
				{
					//ここで決定
					SetCharacterUpdateFunc(static_cast<CharacterUpdateKind>(random), m_characterDrawInfo[number].kind);
					break;
				}
				random++;
			}
		}
	}
	//座標を移動
	m_characterDrawInfo[number].pos += m_characterDrawInfo[number].moveVec;
	//重力をかける
	m_characterDrawInfo[number].moveVec.y += kSideJumpGravity;
}

void TitleUi::BackFryUpdate(int number)
{
	m_characterDrawInfo[number].moveTime--;
	//一定時間が画面外の判定をしない
	if (m_characterDrawInfo[number].moveTime < 0)
	{
		//画面外に出たら
		if (!CheckPos(m_characterDrawInfo[number].pos))
		{
			//更新関数をランダムで設定
			int random = 0;
			int prob = GetRand(kUpdateProbTotal);
			//ランダム処理
			for (auto item : kUpdateProb)
			{
				prob -= item;
				if (prob <= 0)
				{
					//ここで決定
					SetCharacterUpdateFunc(static_cast<CharacterUpdateKind>(random), m_characterDrawInfo[number].kind);
					break;
				}
				random++;
			}
		}
	}

	//座標を移動
	m_characterDrawInfo[number].pos += m_characterDrawInfo[number].moveVec;
	//回転度を変更
	m_characterDrawInfo[number].rota += m_characterDrawInfo[number].rotaSpeed;
}
