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

    void Initialize(StageObjectType objectType, const Vector2& translate, const Vector2& size);

    //--------- accessor -----------------------------------------------------

    void SetObjectCommonState(StageObjectCommonState state) { object_->SetCommonState(state); }

    StageObjectType GetStageObjectType() const { return objectType_; }
    const Vector2& GetBlockTranslate() const { return object_->GetTranslate(); }
    Player* GetPlayer() const;
private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

    // タイプ
    StageObjectType objectType_;
    // IStageObjectを継承したオブジェクトのインスタンスを持つ
    std::unique_ptr<IStageObject> object_ = nullptr;

    //--------- functions ----------------------------------------------------

    // helper
    std::unique_ptr<IStageObject> CreateInstance(StageObjectType objectType) const;
};