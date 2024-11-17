#pragma once
#include <list>
#include <memory>
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

private:

	std::list<std::shared_ptr<Effect>> m_effects;
};

