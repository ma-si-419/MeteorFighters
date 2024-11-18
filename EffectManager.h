#pragma once
#include <list>
#include <map>
#include <memory>
#include <string>
#include "Vector3.h"

class Effect;
class EffectManager
{
public:


	/// <summary>
	/// エフェクトを登録する
	/// </summary>
	/// <param name="effect">登録するエフェクトのポインタ</param>
	/// <param name="pos">エフェクトの座標</param>
	void Entry(std::shared_ptr<Effect> effect,MyEngine::Vector3 pos);

	/// <summary>
	/// 登録しているエフェクトを削除する
	/// </summary>
	/// <param name="effect">削除するエフェクトのポインタ</param>
	void Exit(std::shared_ptr<Effect> effect);

	/// <summary>
	/// エフェクトの更新を行う
	/// </summary>
	void Update();

	/// <summary>
	/// エフェクトの描画を行う
	/// </summary>
	void Draw();
private:

	std::list<std::shared_ptr<Effect>> m_effects;

	std::map<std::string, int> m_handles;
};

