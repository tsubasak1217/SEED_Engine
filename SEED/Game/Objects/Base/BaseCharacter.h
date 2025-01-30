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
    void BeginFrame() override;
    void EndFrame() override;

protected:
    void EndFrameJumpFlagUpdate();

public:// Stateから呼び出す関数
    void ChangeState(ICharacterState* nextState);
    virtual void HandleMove(const Vector3& acceleration);
    virtual void HandleRotate(const Vector3& rotate);

public: // コライダー
    void HandOverColliders() override;
    void InitColliders(ObjectType objectType) override;
    void DiscardPreCollider() override;

protected:
    virtual void Damage(int32_t damage);

public: // アクセッサ
    const Vector3& GetPrePos() const{ return prePos_; }
    void SetTranslate(const Vector3& pos){ model_->translate_ = pos; }
    // アニメーション関連
    void SetAnimation(const std::string& animName,bool loop){ model_->StartAnimation(animName,loop); }
    void SetAnimation(int32_t animIdx,bool loop){ model_->StartAnimation(animIdx,loop); }
    void SetAnimationSpeedRate(float rate){ model_->SetAnimationSpeedRate(rate); }
    float GetAnimationDuration() const{ return model_->GetAnimationDuration(); }
    bool GetIsEndAnimation() const{ return model_->GetIsEndAnimation(); }
    // HPに関連するアクセッサ
    bool GetIsDamaged()const{ return isDamaged_; }
    bool GetIsAlive()const{ return isAlive_; }
    void SetIsAlive(bool isAlive){ isAlive_ = isAlive; }
    int32_t GetHP()const{ return HP_; }
    void SetUnrivalledTime(float time){ unrivalledTime_ = time; }
    float GetUnrivalledTime()const{ return unrivalledTime_; }
    // ジャンプに関連するアクセッサ
    bool GetIsJump()const{ return isJump_; }
    void SetIsJump(bool isJump){ isJump_ = isJump; }
    bool IsJumpable(){ return jumpAllowableTime_ > 0.0f; }
    float GetJumpPower()const{ return jumpPower_; }
    void SetJumpPower(float power){ jumpPower_ = power; }
    void SetIsMovable(bool isMovable){ isMovable_ = isMovable; }
    bool GetIsMovable()const{ return isMovable_; }

protected: // パラメータ
    bool isAlive_ = true;
    int32_t kMaxHP_ = 100;
    int32_t HP_;
    bool isDamaged_ = false;
    float unrivalledTime_ = 0.0f;

protected:// 移動関連
    bool isJump_ = false;
    float jumpPower_ = 0.0f;
    float jumpAllowableTime_ = 0.0f;
    bool isMovable_ = true;

protected:// 状態管理用
    std::unique_ptr<ICharacterState> currentState_ = nullptr;

protected: // 前フレームでの情報
    Vector3 prePos_;
};