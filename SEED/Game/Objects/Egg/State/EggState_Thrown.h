#pragma once

#include "Base/ICharacterState.h"

class EggState_Thrown :
    public ICharacterState{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="_throwDirection">卵が飛んでいく方向</param>
    EggState_Thrown(BaseCharacter* _egg,const Vector3& _velocity);
    ~EggState_Thrown();
    void Initialize(BaseCharacter* character)override;
    void Update()override;
    void Draw()override;
protected:
    void MoveThrow();

    // ステート管理
    void ManageState()override;

private:
    Vector3 velocity_ = {0.0f,0.0f,0.0f};
};