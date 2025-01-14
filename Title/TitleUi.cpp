#include "TitleUi.h"
#include "DxLib.h"
#include <string>
#include "GraphManager.h"
#include "Game.h"

namespace
{
	//フォントの名前
	const TCHAR* kFontName = "GN-キルゴUかなNB";

	//文字の大きさ
	constexpr int kFontSize = 64;

	//カメラのNearFar
	constexpr float kCameraNear = 0.1f;
	constexpr float kCameraFar = 100.0f;

	//PRESSANYBUTTONを表示する座標
	constexpr int kStringPosX = 70;
	constexpr int kStringPosY = 700;

	//表示する文字列
	const std::string kUiString = "PRESS \n    ANY BUTTON";

	//文字がついている時間
	constexpr int kStringExistTime = 90;

	//文字がついていない時間
	constexpr int kStringNotExistTime = 15;

	//マウスを表示する最初の座標
	const MyEngine::Vector2 kMouseInitPos(900, 0);

	//ブルーヘッドを表示する最初の座標
	const MyEngine::Vector2 kBigBlueInitPos(Game::kWindowWidth, 150);

	//最大何フレームキャラクターが頭を出すか
	constexpr int kMaxCharacterMoveFrame = 100;

	//キャラクターの移動速度
	constexpr int kCharacterMoveSpeed = 10;

	//キャラクターの画像の幅
	constexpr int kCharacterSize = 120;
}

TitleUi::TitleUi() :
	m_skyDomeHandle(-1),
	m_time(0),
	m_mouseMoveTime(-1),
	m_blueHeadMoveTime(-1),
	m_isExistString(true)
{
	m_mouseSide = Side::kUp;
	m_blueHeadSide = Side::kRight;

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

	//キャラクターの座標設定
	m_mousePos = kMouseInitPos;
	m_blueHeadPos = kBigBlueInitPos;


	//キャラクターが移動するまでの時間設定
	m_mouseMoveTime = GetRand(kMaxCharacterMoveFrame);
	m_blueHeadMoveTime = GetRand(kMaxCharacterMoveFrame);
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


	//キャラクターの時間計測関係
	m_mouseMoveTime--;
	m_blueHeadMoveTime--;

	//移動する時間になったら
	if (m_mouseMoveTime < 0)
	{
		MyEngine::Vector2 moveVec = GetReturnMoveVec(m_mouseSide);

		m_mousePos += moveVec;

		//画面外に出たかどうかをチェック

		//出ていたら座標をランダムで画面外に設定する
		if (!CheckPos(m_mousePos))
		{

			m_mouseSide = static_cast<Side>(GetRand(static_cast<int>(Side::kSideNum)));

			m_mousePos = SetRandomPos(m_mouseSide);

			m_mouseMoveTime = GetRand(kMaxCharacterMoveFrame);
		}
	}
	//それ以外
	else
	{
		MyEngine::Vector2 moveVec = GetEnterMoveVec(m_mouseSide);

		m_mousePos += moveVec;
	}

	//移動する時間になったら
	if (m_blueHeadMoveTime < 0)
	{
		MyEngine::Vector2 moveVec = GetReturnMoveVec(m_blueHeadSide);

		m_blueHeadPos += moveVec;

		//画面外に出たかどうかをチェック

		//出ていたら座標をランダムで画面外に設定する
		if (!CheckPos(m_blueHeadPos))
		{
			m_blueHeadSide = static_cast<Side>(GetRand(static_cast<int>(Side::kSideNum)));

			m_blueHeadPos = SetRandomPos(m_blueHeadSide);

			m_blueHeadMoveTime = GetRand(kMaxCharacterMoveFrame);
		}
	}
	//それ以外
	else
	{
		MyEngine::Vector2 moveVec = GetEnterMoveVec(m_blueHeadSide);

		m_blueHeadPos += moveVec;
	}

	m_mousePos = ClampPos(m_mouseSide, m_mousePos);

	m_blueHeadPos = ClampPos(m_blueHeadSide, m_blueHeadPos);


}

void TitleUi::Draw()
{
	auto& manager = GraphManager::GetInstance();

	//スカイドームの描画
	MV1DrawModel(m_skyDomeHandle);

	if (m_isExistString)
	{
		DrawStringToHandle(kStringPosX, kStringPosY, kUiString.c_str(), GetColor(255, 255, 255), m_fontHandle, GetColor(0, 0, 0));
	}

	DrawGraph(0, 0, manager.GetHandle("TitleLogo"), true);

	DrawCharacter(m_mouseSide, m_mousePos, manager.GetHandle("Mouse"));
	DrawCharacter(m_blueHeadSide, m_blueHeadPos, manager.GetHandle("BlueHead"));

}

void TitleUi::DrawCharacter(Side side, MyEngine::Vector2 pos, int handle)
{
	double rota = 0.0;

	if (side == Side::kUp)
	{
		//180度回転
		rota = DX_PI;
	}
	else if (side == Side::kRight)
	{
		//270度回転
		rota = (DX_PI / 180) * 270;
	}
	else if (side == Side::kLeft)
	{
		//90度回転
		rota = (DX_PI / 2);
	}
	else if (side == Side::kDown)
	{
		rota = 0.0;
	}

	DrawRotaGraph(pos.x, pos.y, 1.0, rota, handle, true);
}

MyEngine::Vector2 TitleUi::GetEnterMoveVec(Side side)
{
	if (side == Side::kUp)
	{
		return MyEngine::Vector2(0, kCharacterMoveSpeed);
	}
	else if (side == Side::kRight)
	{
		return MyEngine::Vector2(-kCharacterMoveSpeed, 0);
	}
	else if (side == Side::kLeft)
	{
		return MyEngine::Vector2(kCharacterMoveSpeed, 0);
	}
	else if (side == Side::kDown)
	{
		return MyEngine::Vector2(0, -kCharacterMoveSpeed);
	}

	return MyEngine::Vector2(0, 0);
}

MyEngine::Vector2 TitleUi::GetReturnMoveVec(Side side)
{
	if (side == Side::kUp)
	{
		return MyEngine::Vector2(0, -kCharacterMoveSpeed);
	}
	else if (side == Side::kRight)
	{
		return MyEngine::Vector2(kCharacterMoveSpeed, 0);
	}
	else if (side == Side::kLeft)
	{
		return MyEngine::Vector2(-kCharacterMoveSpeed, 0);
	}
	else if (side == Side::kDown)
	{
		return MyEngine::Vector2(0, kCharacterMoveSpeed);
	}

	return MyEngine::Vector2(0, 0);
}

MyEngine::Vector2 TitleUi::ClampPos(Side side, MyEngine::Vector2 pos)
{
	MyEngine::Vector2 ans = pos;

	if (side == Side::kUp)
	{
		ans.y = min(pos.y, kCharacterSize);
	}
	else if (side == Side::kRight)
	{
		ans.x = max(pos.x, Game::kWindowWidth - kCharacterSize);
	}
	else if (side == Side::kLeft)
	{
		ans.x = min(pos.x, kCharacterSize);
	}
	else if (side == Side::kDown)
	{
		ans.y = max(pos.y, Game::kWindowHeight - kCharacterSize);
	}

	return ans;
}

MyEngine::Vector2 TitleUi::SetRandomPos(Side side)
{
	if (side == Side::kUp)
	{
		int randomX = GetRand(Game::kWindowWidth - (kCharacterSize / 2)) + kCharacterSize;
		return MyEngine::Vector2(randomX, -kCharacterSize);
	}
	else if (side == Side::kRight)
	{
		int randomY = GetRand(Game::kWindowHeight - (kCharacterSize / 2)) + kCharacterSize;
		return MyEngine::Vector2(Game::kWindowWidth + kCharacterSize, randomY);
	}
	else if (side == Side::kLeft)
	{
		int randomY = GetRand(Game::kWindowHeight - (kCharacterSize / 2)) + kCharacterSize;
		return MyEngine::Vector2(-kCharacterSize, randomY);
	}
	else if (side == Side::kDown)
	{
		int randomX = GetRand(Game::kWindowWidth - (kCharacterSize / 2)) + kCharacterSize;
		return MyEngine::Vector2(randomX, Game::kWindowHeight + kCharacterSize);
	}


	return MyEngine::Vector2(0, 0);
}

bool TitleUi::CheckPos(MyEngine::Vector2 pos)
{

	if (pos.x >= -kCharacterSize &&
		pos.x <= Game::kWindowWidth + kCharacterSize &&
		pos.y >= -kCharacterSize &&
		pos.y <= Game::kWindowHeight + kCharacterSize)
	{
		return true;
	}
	return false;
}
