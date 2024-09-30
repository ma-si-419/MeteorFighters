#pragma once
#include<memory>
class SceneBase;
class SceneManager
{
public:
	//�R���X�g���N�^
	SceneManager();
	//�f�X�g���N�^
	~SceneManager();

	//����������
	void Init();
	//�X�V����
	void Update();
	//�`�揈��
	void Draw();
	//�ʂ̃V�[���Ɉړ�����ۂɎg�p����֐�
	void ChangeScene(std::shared_ptr<SceneBase> next);
	//�I���������ĂԂƂ��Ɏg��
	void GameEnd() { m_isEnd = true; }
	//�I���������Ă΂ꂽ���ǂ����擾����
	bool GetEnd() { return m_isEnd; }
private:
	std::shared_ptr<SceneBase> m_pScene;//���݂̃V�[��

	std::shared_ptr<SceneBase> m_pNextScene;//���ɍs���V�[����ۑ�����

	bool m_isEnd;//�I���������ĂԂƂ��Ɏg��

	bool m_isFadeOut;//�t�F�[�h�A�E�g���邩

	bool m_isChangeScene;//�V�[����؂�ւ���Ƃ���true�ɂ���

	int m_fadeAlpha;//�t�F�[�h�ŕ\�����鍕���{�b�N�X�̃��l
};