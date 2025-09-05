#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Objects/Interface/IStageObject.h>
#include <cstdint>
#include <unordered_map>

//============================================================================
//	LaserLauncher class
//============================================================================
class LaserLauncher : public IStageObject {
public:
    LaserLauncher(GameObject2D* owner) : IStageObject(owner) {}
    LaserLauncher() = default;
    ~LaserLauncher() = default;
    void Initialize() override;
    void Update() override;

    void Draw() override;

    void Edit() override {};

private:

    //--------- structure ----------------------------------------------------

    // 発射方向
    enum class Direction {
        Up,
        Down,
        Left,
        Right,
    };

public:

    //--------- accessor -----------------------------------------------------

    // 発射方向ごとの本数をセット
    void SetEmitDirections(const std::unordered_map<Direction, int>& directionsCount);
    // 現在の設定を取得
    const std::unordered_map<Direction, int>& GetEmitDirections() const;

private:
    //========================================================================
    //	private Methods
    //========================================================================

    

    //--------- variables ----------------------------------------------------

    // 各方向に何本レーザーを発射するか
    std::unordered_map<Direction, int> emitDirectionsCount_;
    // レーザー発射フラグ
    bool isEmit_ = false;

    //--------- functions ----------------------------------------------------
};