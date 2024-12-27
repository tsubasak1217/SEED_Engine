#pragma once
#include <list>
#include <vector>
#include "Base/BaseCharacter.h"
#include "Collision/Collider.h"
#include "Camera.h"

class IPlayerState;

class Player : public BaseCharacter{
public:
    Player();
    ~Player();
    void Initialize() override;
    void Update() override;
    void Draw() override;

public:// Stateから呼び出す関数
    void HandleMove(const Vector3& acceleration)override;

public:// アクセッサ
    void SetFollowCameraPtr(Camera* pCamera){pCamera_ = pCamera;}
    const Camera* GetFollowCamera()const{ return pCamera_; }

private:// フォローカメラ、ターゲット用
    Camera* pCamera_ = nullptr;

};