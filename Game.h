#pragma once
namespace Game
{
	//��ʂ̉���
	constexpr int kWindowWidth = 1600;
	//��ʂ̏c��
	constexpr int kWindowHeight = 900;

	enum class Scene
	{
		kTitle,
		kMenu,
		kSelect,
		kGame,
		kTutorial
	};
}