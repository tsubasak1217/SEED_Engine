#pragma once

// parent
#include "Base/BaseCharacter.h"
///stl
// pointer
#include <memory>
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
class EnemyManager;
class PredationRange;

class Player
    : public BaseCharacter{
public:
    Player();
    ~Player();
    void Initialize() override;
    void Update() override;
    void Draw() override;

    void EndFrame() override;

    void Spawn(const Vector3& pos);

    void ToClearStageState(const Vector3& nextStartPos);
private:

public: // Stateから呼び出す関数
    void HandleMove(const Vector3& acceleration) override;

    bool CanEatEnemy();

public: // アクセッサ
    void SetPosition(const Vector3& pos){ model_->translate_ = pos; }

    void SetFollowCameraPtr(BaseCamera* pCamera){ pCamera_ = pCamera; }
    const BaseCamera* GetFollowCamera() const{ return pCamera_; }
    BaseCamera* GetFollowCamera(){ return pCamera_; }

    PlayerCorpseManager* GetCorpseManager(){ return corpseManager_; }
    void SetCorpseManager(PlayerCorpseManager* corpseManager){ corpseManager_ = corpseManager; }

    EnemyManager* GetEnemyManager(){ return enemyManager_; }
    void SetEnemyManager(EnemyManager* enemyManager){ enemyManager_ = enemyManager; }

    EggManager* GetEggManager(){ return eggManager_; }
    void SetEggManager(EggManager* eggManager){ eggManager_ = eggManager; }

    PredationRange* GetPredationRange(){ return predationRange_.get(); }

    // 禁忌
    void SetPrePos(const Vector3& _newPrePos){ prePos_ = _newPrePos; }
    void SetLastOnGroundPos(const Vector3& _newLastPosOnGround){ lastPosOnGround_ = _newLastPosOnGround; }
public:
    void OnCollision(const BaseObject* other,ObjectType objectType) override;

private: // フォローカメラ、ターゲット用
    BaseCamera* pCamera_ = nullptr;
    // 死体の管理(外部から設定) 死体を追加するために所持
    PlayerCorpseManager* corpseManager_ = nullptr;
    // 卵の管理(外部から設定) 卵を追加&投げるために所持
    EggManager* eggManager_ = nullptr;
    // 敵の管理(外部から設定) 敵を捕食するために所持
    EnemyManager* enemyManager_ = nullptr;
    // 捕食可能範囲 兼 捕食可能オブジェクトの検索,管理
    std::unique_ptr<PredationRange> predationRange_ = nullptr;

    // 最後に地面についていた位置
    Vector3 lastPosOnGround_;
};