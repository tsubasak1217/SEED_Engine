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

    // 予約されたレーザーの停止を実行する
    void ApplyPendingWarpStop();
    void ClearPendingWarpStop();

    // 衝突無視するワープを設定
    void IgnoreWarpUntilExit(uint32_t index);
    void ResetWarpIgnore();

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

    //--------- structure ----------------------------------------------------

    // 衝突相手
    struct Blocker {

        bool isFound = false;
        Vector2 translate;
        Vector2 size;
        float frontDistance = std::numeric_limits<float>::infinity();
    };

    // 停止予約用
    struct PendingStop {

        bool has = false;
        bool byCenter = false;
        Vector2 translate;
        Vector2 size;
    };

    //--------- variables ----------------------------------------------------

    // タイプ
    LaserObjectType objectType_;
    // ILaserObjectを継承したオブジェクトのインスタンスを持つ
    std::unique_ptr<ILaserObject> object_ = nullptr;

    // 衝突相手
    Blocker blocker_;
    Blocker savedBlocker_;
    bool hasSavedBlocker_ = false;
    // 停止予約用
    PendingStop pendingWarpStop_;
    bool keepStoppedByWarp_ = false;
    // 無効ワープの記録
    bool ignoreWarpUntilExit_ = false;
    uint32_t ignoreWarpIndex_;
    // 1番近いワープ
    float nearestWarpDistance_ = 0.0f;

    //--------- functions ----------------------------------------------------

    // helper
    std::unique_ptr<ILaserObject> CreateInstance(LaserObjectType objectType) const;
    bool CheckEndExtend(GameObject2D* other);
    bool CheckWarp(GameObject2D* other);
    // フィールドオブジェクトと衝突時にレーザーをその場で止める
    void ConsiderBlocker(const Vector2& center, const Vector2& size, bool apply);
    // 予約開始
    void SetPendingWarpStop(const Vector2& translate, const Vector2& size);
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