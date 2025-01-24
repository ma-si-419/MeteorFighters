#pragma once
#include <string>
#include <map>

class LoadManager
{
public:

	enum class FileKind
	{
		kModel,
		kSound,
		kImage,
		kEffect
	};

private:

	enum class BallKind
	{
		kOrange,
		kBlue,
		kGreen,
		kYellow,
		kBallNum
	};

	struct LoadData
	{
		FileKind kind = FileKind::kModel;

		std::string path = "empty";

		int handle = -1;
	};

	struct BallData
	{
		int posX = 0;
		int posY = 0;
		int speed = 0;
		double rota = 0.0;
		int handle = -1;
	};

private:
	LoadManager() = default;
	~LoadManager();
	LoadManager(const LoadManager&) = delete;
	LoadManager& operator = (const LoadManager&) = delete;
	LoadManager(LoadManager&&) = delete;
	LoadManager& operator = (LoadManager&&) = delete;

public:

	static LoadManager& GetInstance()
	{
		static LoadManager instance;
		return instance;
	}

	void Init();

	void Update();

	void Draw();

	void LoadHandle(std::string name, std::string path, FileKind kind);

	int GetHandle(std::string name);

	void StartAsyncLoad();

	void EndAsyncLoad();

	/// <summary>
	/// ロードが終了しているかどうかを返す
	/// </summary>
	/// <returns>非同期ロードが行われているかどうか</returns>
	bool IsEndLoad();

private:

	int Load(std::string path, FileKind kind);
	
private:

	bool m_isAsyncLoad = false;

	std::map<std::string, LoadData> m_handles;

	int m_alpha = 0;

	int m_fontHandle = -1;

	BallData m_ballGraphs[static_cast<int>(BallKind::kBallNum)];
};
