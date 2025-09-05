#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Objects/Interface/IStageObject.h>

//============================================================================
//	BlockNormal class
//============================================================================
class BlockNormal :
    public IStageObject {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    BlockNormal(GameObject2D* owner) : IStageObject(owner){}
    BlockNormal() = default;
    ~BlockNormal() = default;

    void Initialize() override;

    void Update() override;

    void Draw() override;

    void Edit() override {};

    //--------- collision ----------------------------------------------------
    void OnCollisionEnter([[maybe_unused]] GameObject2D* other) override;
    void OnCollisionStay([[maybe_unused]] GameObject2D* other) override;
    void OnCollisionExit([[maybe_unused]] GameObject2D* other) override;

    //--------- accessor -----------------------------------------------------
    void SetSize(const Vector2& size) override;

private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

    Quad2D centerQuad_;
    Quad2D hologramQuad_;
    bool isTouchedByPlayer_ = false;// プレイヤーに触れられているかどうか
    float touchedTimer_ = 0.0f;// プレイヤーに触れられてからの時間
    float motionTimer = 0.0f;// 動作用タイマー
    static inline const float maxTouchedTime_ = 0.25f;// プレイヤーに触れられてからの最大時間
    static inline std::unordered_map<std::string, uint32_t> imageMap_{};

    //--------- functions ----------------------------------------------------
    void QuadDraw();
};