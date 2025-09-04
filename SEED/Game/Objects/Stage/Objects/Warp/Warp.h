#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Objects/Interface/IStageObject.h>

//============================================================================
//	Warp class
//============================================================================
class Warp :
    public IStageObject {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Warp(GameObject2D* owner) : IStageObject(owner) {}
	~Warp() = default;

    void Initialize(const std::string& filename) override;

    void Update() override;

    void Draw() override;

    void Edit() override {};

	//--------- accessor -----------------------------------------------------

    // 自身のワープインデックスを作成時に設定
    void SetWarpIndex(uint32_t warpIndex);
    // 通知を行わせる
    void SetNotification() { currentState_ = State::Notification; }

    uint32_t GetWarpIndex() const { return warpIndex_; }
    bool IsStateNotification()const { return currentState_ == State::Notification; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

    // 状態
    enum class State {

        None,         // 通常更新中(ワープ可能)
        Notification, // ワープ通知
        Warping,      // ワープ中
        Warped,       // ワープ完了
    };

	//--------- variables ----------------------------------------------------

    // 現在の状態
    State currentState_;

    // 自身のワープインデックス
    uint32_t warpIndex_;

	//--------- functions ----------------------------------------------------

    // update
    void UpdateState();
    void UpdateAlways();
};