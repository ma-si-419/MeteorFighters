#pragma once
#include "CharacterStateBase.h"
class CharacterStateJump : public CharacterStateBase,public std::enable_shared_from_this<CharacterStateJump>
{
public:

	CharacterStateJump(std::shared_ptr<Character> character);

	/// <summary>
	/// 左右前後移動ベクトルのあるジャンプを行うときに呼ぶ
	/// </summary>
	/// <param name="moveVec">今の移動ベクトル</param>
	void StartJump(MyEngine::Vector3 moveVec);

	/// <summary>
	/// 左右前後移動がないジャンプを行う時に呼ぶ
	/// </summary>
	void StartJump();

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

private:

	std::string m_attackKey;

	int m_attackButtonPushTime;

	MyEngine::Vector3 m_moveVec;

};

