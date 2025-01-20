#pragma once

#include "Base/ICharacterState.h"

//object
class Egg;

/// <summary>
/// ステージをクリアして, 次のステージに進むステート
/// </summary>
class PlayerStage_ForNextStage
    : public ICharacterState{
public:
    PlayerStage_ForNextStage(const Vector3& nextStartPos,BaseCharacter* player);
    ~PlayerStage_ForNextStage()override;

    void Initialize(
        const std::string& _stateName,
        BaseCharacter* _player)override;

    void Update()override;
    void Draw()override;

protected:
    // ステート管理
    void ManageState()override;
private:
    Egg* egg_ = nullptr;
    Vector3 beforePos_;
    // 次のステージのスタート地点
    Vector3 nextStartPos_;

    Vector3 ctlPoint_;

    float moveTimeMax_ = 1.0f;
    float moveTime_ = 0.0f;

    bool isThrow_ = false;
    bool preIsThrow_ = false;
};