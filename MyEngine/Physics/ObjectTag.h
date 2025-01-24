#pragma once

// 当たったものの判別を行うためのタグ
enum class ObjectTag
{
	kOnePlayer,			//1P
	kTwoPlayer,			//2P
	kOnePlayerAttack,	//1Pの攻撃
	kTwoPlayerAttack,	//2Pの攻撃
	kStage,				//ステージ
	kStageObject		//ステージとしかぶつからないもの
};