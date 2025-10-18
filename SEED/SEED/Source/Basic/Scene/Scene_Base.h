#pragma once
#include <memory>
#include <SEED/Source/Basic/Scene/SceneState_Base.h>
#include <SEED/Source/Manager/Hierarchy/Hierarchy.h>
#include <SEED/Source/Manager/PostEffectSystem/PostEffectSystem.h>
#include <SEED/Source/Manager/EffectSystem/EffectSystem.h>
#include <SEED/Source/SEED.h>

class GameObject;
class GameObject2D;

class Scene_Base{
    friend class GameSystem;
public:
    Scene_Base();
    virtual ~Scene_Base(){};
    virtual void Initialize();
    virtual void Finalize();
    virtual void Update();
    virtual void Draw();
    virtual void BeginFrame();
    virtual void EndFrame();
    virtual void HandOverColliders();

protected:
    virtual void SceneEdit(){};

public:
    void ChangeScene(const std::string& nextSceneName);
    void ChangeState(SceneState_Base* nextState);
    void CauseEvent(SceneState_Base* nextEventState);
    void EndEvent() { currentEventState_ = nullptr; };
    bool HasEvent(){ return currentEventState_ != nullptr; };

public:// Hierarchy関連
    void RegisterToHierarchy(GameObject* gameObject);
    void RegisterToHierarchy(GameObject2D* gameObject);
    void RemoveFromHierarchy(GameObject* gameObject);
    void RemoveFromHierarchy(GameObject2D* gameObject);
    void EraseFromHierarchy(GameObject* gameObject);
    void EraseFromHierarchy(GameObject2D* gameObject);
    Hierarchy* GetHierarchy(){ return hierarchy_.get(); }
    bool IsExistObject(uint32_t gameObjectHanle) const;
    bool IsExistObject2D(uint32_t gameObjectHanle) const;
    void SortObject2DByTranslate(ObjSortMode sortMode);
    GameObject* GetGameObject(uint32_t id) const;
    GameObject2D* GetGameObject2D(uint32_t id) const;
    GameObject* GetGameObject(const std::string& name) const;
    GameObject2D* GetGameObject2D(const std::string& name) const;

protected:
    std::unique_ptr<SceneState_Base> currentState_;
    std::unique_ptr<SceneState_Base> currentEventState_;
    std::unique_ptr<Hierarchy> hierarchy_; // Hierarchy管理クラス
    static inline bool isChangeScene_ = false;
};