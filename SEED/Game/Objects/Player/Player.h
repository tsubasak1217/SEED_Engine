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
class PlayerCorpseManager;
class EggManager;

class Player : public BaseCharacter{
public:
    Player();
    ~Player();
    void Initialize() override;
    void Update() override;
    void Draw() override;

    void Spawn(const Vector3& pos);

    void ToClearStageState(const Vector3& nextStartPos);
private:

public: // Stateから呼び出す関数
    void HandleMove(const Vector3& acceleration) override;

public: // アクセッサ
    void SetPosition(const Vector3& pos){ model_->translate_ = pos; }

    void SetFollowCameraPtr(BaseCamera* pCamera){ pCamera_ = pCamera; }
    const BaseCamera* GetFollowCamera() const{ return pCamera_; }

    PlayerCorpseManager* GetCorpseManager(){ return corpseManager_; }
    void SetCorpseManager(PlayerCorpseManager* corpseManager){ corpseManager_ = corpseManager; }

    EggManager* GetEggManager(){ return eggManager_; }
    void SetEggManager(EggManager* eggManager){ eggManager_ = eggManager; }

public:
    void OnCollision(const BaseObject* other,ObjectType objectType) override;

private: // フォローカメラ、ターゲット用
    BaseCamera* pCamera_ = nullptr;

    PlayerCorpseManager* corpseManager_ = nullptr;
    EggManager* eggManager_ = nullptr;
};