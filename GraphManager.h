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
	/// ������
	/// </summary>
	void Init();

	/// <summary>
	/// �V�[���̉摜�n���h�������[�h����
	/// </summary>
	/// <param name="">�V�[���̖��O</param>
	void LoadSceneGraph(std::string sceneName);

	/// <summary>
	/// �摜�̃n���h�����擾����
	/// </summary>
	/// <param name="">�摜�̖��O</param>
	/// <returns>�摜�n���h��</returns>
	int GetHandle(std::string graphName);
	
private:

	//���ׂẲ摜�̃p�X
	std::map<std::string, std::vector<std::pair<std::string, std::string>>> m_pathData;
	//������V�[���̉摜�̃n���h��
	std::map<std::string, int> m_sceneGraphHandle;
};

