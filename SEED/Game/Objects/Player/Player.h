#pragma once
// parent
#include "Base/BaseCharacter.h"

// lib
#include <memory>
#include <list>
#include <vector>

// object
#include "BaseCamera.h"

class Player : public BaseCharacter{
public:
    Player();
    ~Player();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void EndFrame() override;

public: // Stateから呼び出す関数
    void HandleMove(const Vector3& acceleration) override;

public:
    void OnCollision(const BaseObject* other,ObjectType objectType) override;

public: // アクセッサ
    void SetPosition(const Vector3& pos){ model_->translate_ = pos; }
    void SetFollowCameraPtr(BaseCamera* pCamera){ pCamera_ = pCamera; }
    const BaseCamera* GetFollowCamera() const{ return pCamera_; }
    BaseCamera* GetFollowCamera(){ return pCamera_; }

private: // フォローカメラ、ターゲット用
    BaseCamera* pCamera_ = nullptr;

};