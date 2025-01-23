#pragma once
#include "Vector2.h"
struct CharacterDrawInfo;
class TitleUi
{
public:

	/// <summary>
	/// 描画するキャラクターの種類
	/// </summary>
	enum class DrawCharacterKind
	{
		kMouse,
		kBlue,
		kMouse2P,
		kBlue2P,
		kCharacterKindNum
	};

	/// <summary>
	/// キャラクターの描画サイド
	/// </summary>
	enum class DrawCharacterSide
	{
		kUp,
		kRight,
		kDown,
		kLeft,
		kSideNum = kLeft
	};

private:

	/// <summary>
	/// キャラクターの画像の種類
	/// </summary>
	enum class CharacterGraphKind
	{
		kChoki,
		kPar,
		kKindNum = kPar
	};

	/// <summary>
	/// キャラクターのアップデートの種類
	/// </summary>
	enum class CharacterUpdateKind
	{
		kNormal,
		kFall,
		kSideJump,
		kBackFry,
		kJump,
		kUpdateKindNum = kJump
	};

public:

	TitleUi();

	~TitleUi();

	void Init();

	void Update();

	void Draw();

private:

	/// <summary>
	/// キャラクターが画面に入ってくるときの移動ベクトルを取得する
	/// </summary>
	/// <param name="side">キャラクターのいるサイド</param>
	/// <returns>移動ベクトル</returns>
	MyEngine::Vector2 GetEnterMoveVec(DrawCharacterSide side);

	/// <summary>
	/// キャラクターが画面から出ていくときの移動ベクトルを取得する
	/// </summary>
	/// <param name="side">キャラクターのいるサイド</param>
	/// <returns>移動ベクトル</returns>
	MyEngine::Vector2 GetReturnMoveVec(DrawCharacterSide side);

	/// <summary>
	/// キャラクターの座標を制限する
	/// </summary>
	/// <param name="side">キャラクターのいるサイド</param>
	/// <param name="pos">キャラクターの座標</param>
	/// <returns>制限後のキャラクターの座標</returns>
	MyEngine::Vector2 ClampPos(DrawCharacterSide side, MyEngine::Vector2 pos);

	/// <summary>
	/// 指定したサイドにランダムな座標を設定する
	/// </summary>
	/// <param name="side">キャラクターのサイド</param>
	/// <returns>キャラクターの描画座標</returns>
	MyEngine::Vector2 SetRandomPos(DrawCharacterSide side);

	/// <summary>
	/// 画面内にいるかどうかを返す
	/// </summary>
	/// <param name="pos">キャラクターの座標</param>
	/// <returns>画面に移っているのならばtrueを返す</returns>
	bool CheckPos(MyEngine::Vector2 pos);

	/// <summary>
	/// キャラクターの画像ハンドルを取得する
	/// </summary>
	/// <param name="characterKind">キャラクターの種類</param>
	/// <param name="graphKind">画像の種類</param>
	/// <returns>キャラクターの画像ハンドル</returns>
	int GetCharacterHandle(DrawCharacterKind characterKind, CharacterGraphKind graphKind);

	/// <summary>
	/// キャラクターの更新を設定する
	/// </summary>
	/// <param name="kind">更新の種類</param>
	/// <param name="character">キャラクターの種類</param>
	void SetCharacterUpdateFunc(CharacterUpdateKind kind, DrawCharacterKind character);

private:

	using CharacterUpdateFunc = void(TitleUi::*)(int);

private:

	/// <summary>
	/// キャラクターを描画する際に使用する構造体
	/// </summary>
	struct CharacterDrawInfo
	{
		//キャラクターのハンドル
		int handle = -1;
		//キャラクターの座標
		MyEngine::Vector2 pos;
		//キャラクターの移動ベクトル
		MyEngine::Vector2 moveVec;
		//キャラクターの回転度
		double rota = 0.0;
		//キャラクターの拡大率
		double scale = 1.0;
		//キャラクターの回転速度
		double rotaSpeed = 0.0;
		//キャラクターのサイド
		DrawCharacterSide side = DrawCharacterSide::kUp;
		//キャラクターの移動時間
		int moveTime = 0;
		//キャラクターの種類
		DrawCharacterKind kind = DrawCharacterKind::kMouse;
		//キャラクターの更新関数
		CharacterUpdateFunc updateFunc = &TitleUi::NormalUpdate;
	};

private:

	/// <summary>
	/// 通常時の初期化
	/// </summary>
	/// <param name="number">初期化を行うキャラクター</param>
	void InitNormal(int number);

	/// <summary>
	/// 上から落ちてくるときの初期化
	/// </summary>
	/// <param name="number">初期化を行うキャラクター</param>
	void InitFall(int number);

	/// <summary>
	/// 下からジャンプするときの初期化
	/// </summary>
	/// <param name="number">初期化を行うキャラクター</param>
	void InitJump(int number);

	/// <summary>
	/// 横っ飛びするときの初期化
	/// </summary>
	/// <param name="number">初期化を行うキャラクター</param>
	void InitSideJump(int number);

	/// <summary>
	/// 後ろを飛んでいくキャラクター
	/// </summary>
	/// <param name="number">初期化を行うキャラクター</param>
	void InitBackFry(int number);

	/// <summary>
	/// 通常時のアップデート
	/// </summary>
	/// <param name="character">アップデートを行うキャラクター</param>
	void NormalUpdate(int number);

	/// <summary>
	/// 上から落ちてくるときのアップデート
	/// </summary>
	/// <param name="character">アップデートを行うキャラクター</param>
	void FallUpdate(int number);

	/// <summary>
	/// 下からジャンプするときのアップデート
	/// </summary>
	/// <param name="character">アップデートを行うキャラクター</param>
	void JumpUpdate(int number);

	/// <summary>
	/// 横っ飛びするときのアップデート
	/// </summary>
	/// <param name="number">アップデートを行うキャラクター</param>
	void SideJumpUpdate(int number);

	/// <summary>
	/// 後ろを飛んでいくときのアップデート
	/// </summary>
	/// <param name="number">アップデートを行うキャラクター</param>
	void BackFryUpdate(int number);

private:
	//文字がついている時間
	int m_time;

	//文字が表示されているかどうか
	bool m_isExistString;

	//フォントのハンドル
	int m_fontHandle;

	//スカイドームのハンドル
	int m_skyDomeHandle;

	//キャラクターの描画情報
	CharacterDrawInfo m_characterDrawInfo[static_cast<int>(DrawCharacterKind::kCharacterKindNum)];
};

