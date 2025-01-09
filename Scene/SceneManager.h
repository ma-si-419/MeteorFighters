#pragma once
#include<memory>
#include<vector>
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
	//�񓯊����[�h���s���Ƃ��Ɏg�p����
	void SetAsyncLoad(std::vector<int> handles);
	//�I���������ĂԂƂ��Ɏg��
	void GameEnd() { m_isEnd = true; }
	//�I���������Ă΂ꂽ���ǂ����擾����
	bool IsEnd() { return m_isEnd; }
	//�V�[����ύX���Ă��邩�ǂ���
	bool IsChangeScene() { return m_isChangeScene; }
private:
	std::shared_ptr<SceneBase> m_pScene;//���݂̃V�[��

	std::shared_ptr<SceneBase> m_pNextScene;//���ɍs���V�[����ۑ�����

	bool m_isEnd;//�I���������ĂԂƂ��Ɏg��

	bool m_isFadeOut;//�t�F�[�h�A�E�g���邩

	bool m_isChangeScene;//�V�[����؂�ւ���Ƃ���true�ɂ���

	int m_fadeAlpha;//�t�F�[�h�ŕ\�����鍕���{�b�N�X�̃��l

	std::vector<int> m_handles;
};