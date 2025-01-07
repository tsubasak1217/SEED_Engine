#pragma once
#include <list>
#include <vector>
#include "BaseObject.h"
#include "BaseCamera.h"

class ICharacterState;

class BaseCharacter : public BaseObject{
public:
    BaseCharacter();
    ~BaseCharacter();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void EndFrame() override;

public:// Stateから呼び出す関数
    void ChangeState(ICharacterState* nextState);
    virtual void HandleMove(const Vector3& acceleration);
    virtual void HandleRotate(const Vector3& rotate);

protected:
    void HandOverColliders()override;
    void InitColliders()override;

public:// アクセッサ

    const Vector3& GetPrePos()const{ return prePos_; }
    void SetAnimation(const std::string& animName, bool loop){ model_->StartAnimation(animName, loop); }
    void SetAnimation(int32_t animIdx, bool loop){ model_->StartAnimation(animIdx, loop); }
    float GetAnimationDuration()const{ return model_->GetAnimationDuration(); }
    bool GetIsEndAnimation()const{ return model_->GetIsEndAnimation(); }
    bool GetIsDamaged()const{ return isDamaged_; }

protected:// パラメータ
    bool isDamaged_ = false;

protected:// 状態管理用
    std::unique_ptr<ICharacterState> currentState_ = nullptr;

protected:// 前フレームでの情報
    Vector3 prePos_;
};