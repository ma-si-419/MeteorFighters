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

		//画像の名前を保存
		pushData.name = item[static_cast<int>(FileDataSort::kGraphName)];
		//画像のパスを保存
		pushData.path = item[static_cast<int>(FileDataSort::kPath)];
		//画像をどのシーンで使うかを保存
		pushData.scene = item[static_cast<int>(FileDataSort::kSceneName)];

		//一時的にデータを保存
		pathData.push_back(pushData);

		//同じ名前のシーンが登録されていないか
		bool isSame = false;

		for (auto name : sceneName)
		{
			if (name == item[static_cast<int>(FileDataSort::kSceneName)])
			{
				isSame = true;
			}
		}
		//同じ名前が存在していなければ登録
		if (!isSame)
		{
			sceneName.push_back(item[static_cast<int>(FileDataSort::kSceneName)]);
		}
	}

	//保存されたシーンの数だけ回す
	for (auto scene : sceneName)
	{
		std::vector<std::pair<std::string, std::string>> pushData;
		//保存されたパスのデータの数だけ回す
		for (auto item : pathData)
		{
			//シーンの名前とパスのデータのシーンが同じであれば
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
	//画像ハンドルを削除する
	if (m_sceneGraphHandle.size() > 0)
	{
		for (auto& item : m_sceneGraphHandle)
		{
			DeleteGraph(item.second);
		}
	}

	//指定されたシーンのロードするパス
	std::vector<std::pair<std::string, std::string>> loadPaths = m_pathData[sceneName];

	//画像をロードする
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
