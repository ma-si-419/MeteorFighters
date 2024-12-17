#pragma once
#include <map>
#include <vector>
#include <string>
class GraphManager
{
private:

	enum class FileDataSort
	{
		kGraphName,
		kPath,
		kSceneName
	};

private:
	GraphManager() = default;
	~GraphManager();
	GraphManager(const GraphManager&) = delete;
	GraphManager& operator = (const GraphManager&) = delete;
	GraphManager(GraphManager&&) = delete;
	GraphManager& operator = (GraphManager&&) = delete;
public:

	static GraphManager& GetInstance()
	{
		static GraphManager instance;
		return instance;
	}

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// シーンの画像ハンドルをロードする
	/// </summary>
	/// <param name="">シーンの名前</param>
	void LoadSceneGraph(std::string sceneName);

	/// <summary>
	/// 画像のハンドルを取得する
	/// </summary>
	/// <param name="">画像の名前</param>
	/// <returns>画像ハンドル</returns>
	int GetHandle(std::string graphName);
	
private:

	//すべての画像のパス
	std::map<std::string, std::vector<std::pair<std::string, std::string>>> m_pathData;
	//今いるシーンの画像のハンドル
	std::map<std::string, int> m_sceneGraphHandle;
};

