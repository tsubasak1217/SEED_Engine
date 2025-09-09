#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <Game/Objects/Stage/Objects/Interface/IStageObject.h>

// front
class Player;

//============================================================================
//	StageObjectComponent class
//============================================================================
class StageObjectComponent : public IComponent {
public:
    StageObjectComponent(GameObject2D* pOwner, const std::string& tagName = "");
    ~StageObjectComponent() = default;

    void Initialize()override;
    void BeginFrame() override;
    void Update() override;
    void Draw() override;
    void EndFrame() override;
    void Finalize() override;
    // GUI編集
    void EditGUI() override;
    // json
    nlohmann::json GetJsonData() const override;
    void LoadFromJson(const nlohmann::json& jsonData) override;

    void Initialize(
        StageObjectType objectType, const Vector2& translate, const Vector2& size, 
        StageObjectCommonState state = StageObjectCommonState::None);
    // ホログラム発生時のアニメーション
    void AppearanceUpdateAnimation(float baseDuration, float spacing, Easing::Type easing);

    //--------- collision ----------------------------------------------------

    void OnCollisionStay(GameObject2D* other) override;
    void OnCollisionEnter(GameObject2D* other) override;
    void OnCollisionExit(GameObject2D* other) override;

    //--------- accessor -----------------------------------------------------

    void SetObjectCommonState(StageObjectCommonState state) { object_->SetCommonState(state); }
    void SetObjectColum(uint32_t objectColum) { objectColum_ = objectColum; }

    StageObjectType GetStageObjectType() const { return objectType_; }
    const Vector2& GetBlockTranslate() const { return object_->GetTranslate(); }
    void UpdateBlockTranslate(){ object_->SetTranslate(owner_.owner2D->GetWorldTranslate()); }
    const Vector2& GetMapSize() const { return mapSize_; }
    uint32_t GetObjectColum() const { return objectColum_; }

    template <typename T>
    T* GetStageObject() const;
    IStageObject* GetTypeStageObject() const;
private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

    // タイプ
    StageObjectType objectType_;
    // IStageObjectを継承したオブジェクトのインスタンスを持つ
    std::unique_ptr<IStageObject> object_ = nullptr;
    // オブジェクトの列番号
    uint32_t objectColum_;

    // サイズを保持
    Vector2 mapSize_;

    //--------- functions ----------------------------------------------------

    // helper
    std::unique_ptr<IStageObject> CreateInstance(StageObjectType objectType) const;
};

//============================================================================
//	StageObjectComponent templateMethods
//============================================================================

template<typename T>
inline T* StageObjectComponent::GetStageObject() const {

    static_assert(std::is_base_of<IStageObject, T>::value, "T must derive from IStageObject");
    if (!object_) {
        return nullptr;
    }
    return dynamic_cast<T*>(object_.get());
}