#pragma once
#include <list>
#include <vector>
#include <memory>
#include <json.hpp>
#include <SEED/Source/Basic/Object/GameObject.h>

class Hierarchy{

public:
    Hierarchy() = default;
    ~Hierarchy() = default;
    // ゲームオブジェクトの登録・削除
    void RegisterGameObject(GameObject* gameObject);
    void RemoveGameObject(GameObject* gameObject);
    void EraseObject(GameObject* gameObject);

public:
    // フレームの開始時処理
    void BeginFrame();
    // コンポーネントの更新
    void Update();
    // コンポーネントの描画
    void Draw();
    // フレームの終了時処理
    void EndFrame();
    // 終了処理
    void Finalize();

private:
    void RebuildParentInfo();
    void EditGUI();
    void RecursiveTreeNode(GameObject* gameObject, int32_t depth);
    void CreateEmptyObject();
    void InOutOnGUI();
    void OutputPrefab(GameObject* gameObject);
    void ExecuteContextMenu();

public:
    // json
    nlohmann::json OutputToJson(const std::string& outputFilePath, std::list<GameObject*> grandParentObjects) const;
    void LoadFromJson(const std::string& filePath);

private:
    std::list<GameObject*> gameObjects_;// 登録されているゲームオブジェクトのリスト
    std::list<GameObject*> grandParentObjects_;// 親オブジェクトのリスト
    GameObject* selectedObject_ = nullptr; // 選択中のオブジェクト
    GameObject* contextMenuObject_ = nullptr; // コンテキストメニューのオブジェクト
    std::list<std::unique_ptr<GameObject>> selfCreateObjects_; // 自分で生成したオブジェクトのリスト
    std::string executeMenuName_;
};