#pragma once
#include<memory>
#include<vector>
class SceneBase;
class SceneManager
{
public:
	//コンストラクタ
	SceneManager();
	//デストラクタ
	~SceneManager();

	//初期化処理
	void Init();
	//更新処理
	void Update();
	//描画処理
	void Draw();
	//別のシーンに移動する際に使用する関数
	void ChangeScene(std::shared_ptr<SceneBase> next);
	//非同期ロードを行うときに使用する
	void SetAsyncLoad(std::vector<int> handles);
	//終了処理を呼ぶときに使う
	void GameEnd() { m_isEnd = true; }
	//終了処理が呼ばれたかどうか取得する
	bool IsEnd() { return m_isEnd; }
	//シーンを変更しているかどうか
	bool IsChangeScene() { return m_isChangeScene; }
private:
	std::shared_ptr<SceneBase> m_pScene;//現在のシーン

	std::shared_ptr<SceneBase> m_pNextScene;//次に行くシーンを保存する

	bool m_isEnd;//終了処理を呼ぶときに使う

	bool m_isFadeOut;//フェードアウトするか

	bool m_isChangeScene;//シーンを切り替えるときにtrueにする

	int m_fadeAlpha;//フェードで表示する黒いボックスのα値

	std::vector<int> m_handles;
};