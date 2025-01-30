#pragma once

#include <memory>

#include "PlayerState_Move.h"

#include "Vector3.h"

#include "../Objects/Egg/Egg.h"
#include "../Objects/Egg/Manager/EggManager.h"

class PlayerState_ThrowEgg
    :public ICharacterState{
public:
    PlayerState_ThrowEgg(
        const std::string& stateName,
        BaseCharacter* player);
    ~PlayerState_ThrowEgg();

    void Initialize(const std::string& stateName,BaseCharacter* character)override;
    void Update()override;
    void Draw()override;
    void HandOverColliders()override;

private:
    void ManageState()override;

    void UpdateMovingState();
    void ChangeAnimation();
    void Aim();
    void SimulateThrowEgg();
    void UpdateCoontrolPoints();
    void DrawLocus(uint32_t subdivision);

private:
    EggManager* eggManager_ = nullptr;
    Egg* throwEgg_ = nullptr;

    // 最初のアニメーションがloopしないから 他の アニメーションに 切り替えるためのフラグ
    bool isFirstAnimationEnd_ = false;

    // 投げる卵のオフセット
    Vector3 eggOffset_ = {0.0f,0.0f,5.0f};
    // 投げる方向のオフセット
    Vector3 throwDirection_ = {0.0f,1.0f,1.0f};
    // 投げるパワー
    float throwPower_ = 20.0f;
    //投げる卵の重さ
    float eggWeight_ = 1.0f;
    // 狙う関連
    float aimRadius_;
    Vector3 aimPos_;
    std::unique_ptr<Model> aimModel_;
    std::unique_ptr<Model> sphere_;
    std::unique_ptr<BaseObject> throwSimulationEgg_;
    std::array<Vector3, 3> controlPoints_;
    float simulateTime_;

    bool isMoving_ = false;
    bool preIsMoving_ = false;

    // 卵を投げるとき FocusButton を 押していないと行けないか,切り替えか
    bool pressForcus_ = false;
};