#pragma once

#include "Base/ICharacterState.h"

class EggState_Thrown :
    public ICharacterState{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="_throwDirection">卵が飛んでいく方向</param>
    EggState_Thrown(BaseCharacter* _egg,const Vector3& _directionXY,float _rotateY,float _speed);
    ~EggState_Thrown();
    void Initialize(const std::string& stateName,BaseCharacter* character)override;
    void Update()override;
    void Draw()override;
protected:
    void MoveThrow();

    // ステート管理
    void ManageState()override;

private:
    float throwTime_;
    float leftTime_;

    Vector3 beforePos_;
    Vector3 directionXY_;
    float rotateY_;
    float speed_;

    float weight_;
};