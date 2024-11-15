#pragma once

// 当たったものの判別を行うためのタグ
enum class ObjectTag
{
	kOnePlayer,			//プレイヤー
	kTwoPlayer,			//敵
	kPlayerAttack,		//プレイヤーの攻撃
	kEnemyAttack,		//エネミーの攻撃
	kStage				//ステージ
};