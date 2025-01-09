#pragma once

// parent
#include "Base/BaseCharacter.h"
// container
#include <list>
#include <vector>
// object
#include "Collision/Collider.h"
#include "BaseCamera.h"
// state
class IPlayerState;
// manager
class EggManager;

class Player : public BaseCharacter
{
public:
    Player();
    ~Player();
    void Initialize() override;
    void Update() override;
    void Draw() override;

    void Spawn(const Vector3 &pos);

private:
    void InitCollider() override;

public: // Stateから呼び出す関数
    void HandleMove(const Vector3 &acceleration) override;

public: // アクセッサ
    void SetFollowCameraPtr(BaseCamera *pCamera) { pCamera_ = pCamera; }
    const BaseCamera *GetFollowCamera() const { return pCamera_; }

    EggManager *GetEggManager() { return eggManager_; }
    void SetEggManager(EggManager *eggManager) { eggManager_ = eggManager; }

public:
    void OnCollision(const BaseObject *other, ObjectType objectType) override;

private: // フォローカメラ、ターゲット用
    BaseCamera *pCamera_ = nullptr;

    EggManager *eggManager_ = nullptr;
};