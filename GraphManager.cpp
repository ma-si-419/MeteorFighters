#include "GraphManager.h"
#include "LoadCsv.h"
#include "DxLib.h"

GraphManager::~GraphManager()
{
}

void GraphManager::Init()
{
	LoadCsv loadCsv;

	std::vector<std::vector<std::string>> loadData = loadCsv.LoadFile("data/csv/graphData.csv");

	std::vector<std::string> sceneName;

	struct pushDataInfo
	{
		std::string name;
		std::string path;
		std::string scene;
	};

	std::vector<pushDataInfo> pathData;

	for (auto item : loadData)
	{
		pushDataInfo pushData;

		//�摜�̖��O��ۑ�
		pushData.name = item[static_cast<int>(FileDataSort::kGraphName)];
		//�摜�̃p�X��ۑ�
		pushData.path = item[static_cast<int>(FileDataSort::kPath)];
		//�摜���ǂ̃V�[���Ŏg������ۑ�
		pushData.scene = item[static_cast<int>(FileDataSort::kSceneName)];

		//�ꎞ�I�Ƀf�[�^��ۑ�
		pathData.push_back(pushData);

		//�������O�̃V�[�����o�^����Ă��Ȃ���
		bool isSame = false;

		for (auto name : sceneName)
		{
			if (name == item[static_cast<int>(FileDataSort::kSceneName)])
			{
				isSame = true;
			}
		}
		//�������O�����݂��Ă��Ȃ���Γo�^
		if (!isSame)
		{
			sceneName.push_back(item[static_cast<int>(FileDataSort::kSceneName)]);
		}
	}

	//�ۑ����ꂽ�V�[���̐�������
	for (auto scene : sceneName)
	{
		std::vector<std::pair<std::string, std::string>> pushData;
		//�ۑ����ꂽ�p�X�̃f�[�^�̐�������
		for (auto item : pathData)
		{
			//�V�[���̖��O�ƃp�X�̃f�[�^�̃V�[���������ł����
			if (scene == item.scene)
			{
				std::pair<std::string, std::string> data;
				data.first = item.name;
				data.second = item.path;

				pushData.push_back(data);
			}
		}
		m_pathData[scene] = pushData;
	}
}

void GraphManager::LoadSceneGraph(std::string sceneName)
{
	//�摜�n���h�����폜����
	if (m_sceneGraphHandle.size() > 0)
	{
		for (auto& item : m_sceneGraphHandle)
		{
			DeleteGraph(item.second);
		}
	}

	//�w�肳�ꂽ�V�[���̃��[�h����p�X
	std::vector<std::pair<std::string, std::string>> loadPaths = m_pathData[sceneName];

	//�摜�����[�h����
	for (auto item : loadPaths)
	{
		std::string path = "data/image/" + item.second + ".png";

		m_sceneGraphHandle[item.first] = LoadGraph(path.c_str());
	}

}

int GraphManager::GetHandle(std::string graphName)
{
	return m_sceneGraphHandle[graphName];
}
