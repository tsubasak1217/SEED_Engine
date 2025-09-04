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

    uint32_t GetWarpIndex() const { return warpIndex_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

    // 自身のワープインデックス
    uint32_t warpIndex_;

	//--------- functions ----------------------------------------------------

    // update
    void UpdateAlways();
};