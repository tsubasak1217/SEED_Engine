#pragma once

#include "Base/ICharacterState.h"

class Egg;

class EggState_Thrown :
    public ICharacterState{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="_throwDirection">卵が飛んでいく方向</param>
    EggState_Thrown(BaseCharacter* _egg,const Vector3& p1, const Vector3& p2, const Vector3& p3);
    ~EggState_Thrown();
    void Initialize(const std::string& stateName,BaseCharacter* character)override;
    void Update()override;
    void Draw()override;
protected:
    void MoveThrow();

    // ステート管理
    void ManageState()override;

private:
    Egg* pEgg_ = nullptr;

    float throwTime_;
    float leftTime_;

    Vector3 beforePos_;
    std::array<Vector3, 3> controlPoints_;

    float weight_;
};