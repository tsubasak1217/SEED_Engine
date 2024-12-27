#pragma once
#include <list>
#include <vector>
#include "BaseObject.h"
#include "Collision/Collider.h"
#include "Camera.h"

class IState;

class BaseCharacter : public BaseObject{
public:
    BaseCharacter();
    ~BaseCharacter();
    void Initialize() override;
    void Update() override;
    void Draw() override;


public:// Stateから呼び出す関数
    void ChangeState(IState* nextState);
    virtual void HandleMove(const Vector3& acceleration);
    virtual void HandleRotate(const Vector3& rotate);

public:// アクセッサ

    const Vector3& GetPrePos()const{ return prePos_; }
    void SetAnimation(const std::string& animName, bool loop){ model_->StartAnimation(animName, loop); }
    void SetAnimation(int32_t animIdx, bool loop){ model_->StartAnimation(animIdx, loop); }
    float GetAnimationDuration()const{ return model_->GetAnimationDuration(); }

protected:// 衝突判定用
    std::vector<Collider> colliders_;

protected:// 状態管理用
    std::unique_ptr<IState> currentState_ = nullptr;

protected:// 前フレームでの情報
    Vector3 prePos_;
};