#pragma once
#include <list>
#include <vector>
#include "Base/BaseCharacter.h"
#include "Collision/Collider.h"
#include "BaseCamera.h"
#include "Player/Player.h"

class EnemyManager;
class Enemy
    : public BaseCharacter{
public:
    /// <summary>
    /// 
    /// </summary>
    /// <param name="pManager"></param>
    /// <param name="pPlayer"></param>
    /// <param name="enemyName"> インスタンスの ID として扱う</param>
    Enemy(EnemyManager* pManager,Player* pPlayer,const std::string& enemyName);
    ~Enemy();
    void Initialize() override;
    void InitializeRoutine();
    void Update() override;
    void Draw() override;
    void EndFrame() override;

    void ShowImGui();

    // 新規追加: 敵データを JSON に登録するメソッド
    void RegisterDataToJson(const std::string& group, int index);
    void LoadDataFromJson(const std::string& group, int index);
    void SaveData();
    void LoadData();

protected:
    void Damage(int32_t damage)override;

public:
    //--- getter / setter ---//
    float GetDistanceToPlayer()const;
    const Player* GetTargetPlayer()const{ return pPlayer_; }

    const EnemyManager* GetManager()const{ return pManager_; }

    int32_t GetHP()const{ return HP_; }
    void SetHP(int32_t hp){ HP_ = hp; }

    bool GetCanEat()const{ return canEat_; }
    bool GetChasePlayer()const{ return cahsePlayer_; }

    const std::string& GetRoutineName()const{ return routineName_; }
    // ルーチンポイントの取得
    const std::vector<Vector3>& GetRoutinePoints() const{
        return routinePoints_;
    }

    void Rename(const std::string& newName);

    //--- setter !調整項目 ---//
    //追伸 : この setter は、EnemyEditor でのみ使用するすみません
    void SetCanEat(bool canEat){ canEat_ = canEat; }
    void SetChasePlayer(bool chase){ cahsePlayer_ = chase; }
    void SetRoutineName(const std::string& routineName){ routineName_ = routineName; }
    // ルーチンポイントの設定
    void SetRoutinePoints(const std::vector<Vector3>& points){
        routinePoints_ = points;
    }

    void SetMoveSpeed(float speed){ speed_ = speed; }
    float GetMoveSpeed()const{ return speed_; }

public:
    void OnCollision(BaseObject* other,ObjectType objectType)override;

private:
    Player* pPlayer_ = nullptr;
    EnemyManager* pManager_ = nullptr;

    std::string routineName_ = "NULL";

    bool canEat_ = false;
    bool cahsePlayer_ = false;

    std::vector<Vector3> routinePoints_;

    float speed_ = 1.0f;

private:
    int32_t HP_;
};