#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Basic/Components/IComponent.h>
#include <Game/Objects/Stage/Objects/Interface/ILaserObject.h>

// c++
#include <unordered_set>

//============================================================================
//	LaserObjectComponent class
//============================================================================
class LaserObjectComponent :
    public IComponent {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    LaserObjectComponent(GameObject2D* pOwner, const std::string& tagName = "");
    ~LaserObjectComponent() = default;

    //--------- IComponent ---------------------------------------------------

    void Initialize()override {}
    void BeginFrame() override;
    void Update() override;
    void Draw() override;
    void EndFrame() override;
    void Finalize() override {}
    // GUI編集
    void EditGUI() override;
    // json
    nlohmann::json GetJsonData() const override;
    void LoadFromJson(const nlohmann::json& jsonData) override;

    //-------  LaserObject ---------------------------------------------------

    void Initialize(LaserObjectType objectType, const Vector2& translate);

    //--------- collision ----------------------------------------------------

    void OnCollisionEnter(GameObject2D* other) override;
    void OnCollisionStay(GameObject2D* other) override;
    void OnCollisionExit(GameObject2D* other) override;

    //--------- accessor -----------------------------------------------------

    void SetSize(const Vector2& size) { object_->SetSize(size); }

    // 状態の設定
   // 伸び始める
    void ReExtend() { object_->ReExtend(); }
    // 伸びるのを終了
    void StopExtend() { object_->StopExtend(); }
    void SetObjectCommonState(StageObjectCommonState state) { object_->SetCommonState(state); }
    void SetLaserDirection(DIRECTION4 direction) { object_->SetDirection(direction); }

    LaserObjectType GetLaserObjectType() const { return objectType_; }

    template <typename T>
    T* GetLaserObject() const;
private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

    // タイプ
    LaserObjectType objectType_;
    // ILaserObjectを継承したオブジェクトのインスタンスを持つ
    std::unique_ptr<ILaserObject> object_ = nullptr;

    //--------- functions ----------------------------------------------------

    // helper
    std::unique_ptr<ILaserObject> CreateInstance(LaserObjectType objectType) const;
    bool CheckEndExtend(GameObject2D* other);
};

//============================================================================
//	LaserObjectComponent templateMethods
//============================================================================

template<typename T>
inline T* LaserObjectComponent::GetLaserObject() const {

    static_assert(std::is_base_of<ILaserObject, T>::value, "T must derive from ILaserObject");
    if (!object_) {
        return nullptr;
    }
    return dynamic_cast<T*>(object_.get());
}