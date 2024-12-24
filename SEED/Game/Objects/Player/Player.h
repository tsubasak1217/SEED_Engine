#pragma once
#include <list>
#include <vector>
#include "BaseObject.h"
#include "Collision/Collider.h"
#include "Camera.h"

class IPlayerState;

class Player : public BaseObject{
public:
    Player();
    ~Player();
    void Initialize() override;
    void Update() override;
    void Draw() override;


public:// Stateから呼び出す関数
    void ChangeState(IPlayerState* nextState);
    void HandleMove(const Vector3& acceleration);
    void HandleRotate(const Vector3& rotate);

public:// アクセッサ
    void SetFollowCameraPtr(Camera* pCamera){pCamera_ = pCamera;}
    const Camera* GetFollowCamera()const{ return pCamera_; }
    const Vector3& GetPrePos()const{ return prePos_; }

private:// フォローカメラ、ターゲット用
    Camera* pCamera_ = nullptr;

private:// 衝突判定用
    std::vector<Collider> colliders_;
    
private:// 移動関連
    float moveSpeed_ = 30.0f;

private:// 状態管理用
    std::unique_ptr<IPlayerState> currentState_ = nullptr;

private:// 前フレームでの情報
    Vector3 prePos_;
};