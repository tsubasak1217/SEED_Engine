#pragma once
#include <list>
#include <vector>
#include <memory>
#include <json.hpp>
#include "LoadObjectData.h"

enum class ObjSortMode{
    AscendX,
    DescendX,
    AscendY,
    DescendY,
    AcsendZ,
    DescendZ
};

class Hierarchy{

public:
    Hierarchy();
    ~Hierarchy();
    // ゲームオブジェクトの登録・削除
    void RegisterGameObject(GameObject* gameObject);
    void RegisterGameObject(GameObject2D* gameObject);
    void RemoveGameObject(GameObject* gameObject);
    void RemoveGameObject(GameObject2D* gameObject);
    void EraseObject(GameObject* gameObject);
    void EraseObject(GameObject2D* gameObject);

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

public:
    bool IsExistObject(uint32_t id) const;
    bool IsExistObject2D(uint32_t id) const;
    void SortObject2DByTranslate(ObjSortMode sortMode);
    GameObject* GetGameObject(uint32_t id) const;
    GameObject2D* GetGameObject2D(uint32_t id) const;
    GameObject* GetGameObject(const std::string& name) const;
    GameObject2D* GetGameObject2D(const std::string& name) const;

private:
    void RebuildParentInfo();
    void EditGUI();
    void RecursiveTreeNode(GameObject* gameObject, int32_t depth);
    void RecursiveTreeNode(GameObject2D* gameObject, int32_t depth);
    void CreateEmptyObject();
    void InOutOnGUI();
    void OutputPrefab(GameObject* gameObject);
    void OutputPrefab(GameObject2D* gameObject);
    void ExecuteContextMenu();

public:
    // json
    nlohmann::json OutputToJson(std::list<GameObject*> grandParentObjects,bool isSaveOnOrigin = false) const;
    nlohmann::json OutputToJson(std::list<GameObject2D*> grandParentObjects,bool isSaveOnOrigin = false) const;
    LoadObjectData LoadFromJson(const std::string& filePath, bool resetObjects = true);

private:
    std::list<GameObject*> gameObjects_;// 登録されているゲームオブジェクトのリスト
    std::list<GameObject2D*> gameObjects2D_;
    std::list<GameObject*> grandParentObjects_;// 親オブジェクトのリスト
    std::list<GameObject2D*> grandParentObjects2D_;

    GameObject* selectedObject_ = nullptr; // 選択中のオブジェクト
    GameObject2D* selectedObject2D_ = nullptr;
    GameObject* contextMenuObject_ = nullptr; // コンテキストメニューのオブジェクト
    GameObject2D* contextMenuObject2D_ = nullptr;
    std::list<std::unique_ptr<GameObject>> selfCreateObjects_; // 自分で生成したオブジェクトのリスト
    std::list<std::unique_ptr<GameObject2D>> selfCreateObjects2D_;
    std::unordered_set<uint32_t> existObjectIdMap_; // 使用されているIDの集合
    std::unordered_set<uint32_t> existObjectIdMap2D_; // 使用されているIDの集合(2D)
    std::string executeMenuName_;
    static inline bool isEndHierarchy_ = false;
};