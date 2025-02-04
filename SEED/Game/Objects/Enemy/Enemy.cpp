#include "Enemy.h"

//state
#include "EnemyState/EnemyState_Down.h"
#include "EnemyState/EnemyState_Idle.h"
#include "EnemyState/EnemyState_RoutineMove.h"

//manager
#include "EnemyManager.h"

//engine
#include "SEED.h"
//lib
#include "../adapter/json/JsonCoordinator.h"
#include "ParticleManager/ParticleManager.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
Enemy::Enemy(EnemyManager* pManager,Player* pPlayer,const std::string& enemyName){
    className_ = "Enemy";
    name_ = enemyName;
    pPlayer_ = pPlayer;
    pManager_ = pManager;
    Initialize();
}

Enemy::~Enemy(){}

void Enemy::Initialize(){

    // 属性の決定
    objectType_ = ObjectType::Enemy;

    // モデルの初期化
    model_ = std::make_unique<Model>("enemy.gltf");
    model_->UpdateMatrix();
    model_->isRotateWithQuaternion_ = false;

    // 状態の初期化
    currentState_ = std::make_unique<EnemyState_RoutineMove>("Enemy_Idle",this);

    // コライダーの初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_,this);
    InitColliders(ObjectType::Enemy);
    AddSkipPushBackType(ObjectType::Enemy);

    // ターゲットになる際の注目点のオフセット
    targetOffset_ = Vector3(0.0f,3.0f,0.0f);

    // スイッチを押すための重さ
    switchPushWeight_ = 1.0f;

}

void Enemy::InitializeRoutine(){
    const EnemyManager* manager = GetManager();
    const auto* points = manager->GetRoutinePoints(routineName_);
    if (points){
        SetRoutinePoints(*points); // 選択ルーチンに対応するポイントを設定
    }
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void Enemy::Update(){
    BaseCharacter::Update();

    EditCollider();

    // もし無敵時間があれば
    if(unrivalledTime_ > 0.0f){
        unrivalledTime_ -= ClockManager::DeltaTime();
        unrivalledTime_ = std::clamp(unrivalledTime_,0.0f,10000.0f);
    }

}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void Enemy::Draw(){
    BaseCharacter::Draw();
}

//////////////////////////////////////////////////////////////////////////
// エンドフレーム処理
//////////////////////////////////////////////////////////////////////////
void Enemy::EndFrame(){
    BaseCharacter::EndFrame();
}

//////////////////////////////////////////////////////////////////////////
// ImGui
//////////////////////////////////////////////////////////////////////////
void Enemy::ShowImGui(){
    // HP
    ImGui::DragInt("HP", &HP_, 1.0f);

    // canEat
    ImGui::Checkbox("CanEat", &canEat_);

    // chasePlayer
    ImGui::Checkbox("Chase Player", &cahsePlayer_);

    // moveSpeed
    ImGui::DragFloat("Move Speed", &speed_, 0.1f);

    // ルーチン選択用のコンボ
    const EnemyManager* manager = GetManager();
    std::vector<std::string> routineNames = manager->GetRoutineNames();

    // 現在の routineName_ をインデックス化
    int currentIndex = 0;
    for (size_t i = 0; i < routineNames.size(); ++i){
        if (routineNames[i] == routineName_){
            currentIndex = static_cast< int >(i);
            break;
        }
    }

    // コンボ表示
    std::vector<const char*> routineNamesCStr;
    routineNamesCStr.reserve(routineNames.size());
    for (auto& rName : routineNames){
        routineNamesCStr.push_back(rName.c_str());
    }

    if (ImGui::Combo("Select Routine", &currentIndex,
        routineNamesCStr.data(),
        static_cast< int >(routineNamesCStr.size()))){
        // 選択変更されたら更新
        routineName_ = routineNames[currentIndex];
        const auto* points = manager->GetRoutinePoints(routineName_);
        if (points){
            SetRoutinePoints(*points); // 選択ルーチンに対応するポイントを設定
        }
    }

    //状態遷移
    if (ImGui::Button("Change State_Idle")){
        ChangeState(new EnemyState_Idle("Enemy_Idle", this));
    }
    if(ImGui::Button("change State_RoutineMove")){
        ChangeState(new EnemyState_RoutineMove("Enemy_RoutineMove", this));
    }

}


//////////////////////////////////////////////////////////////////////////
// データのセーブ
//////////////////////////////////////////////////////////////////////////
void Enemy::RegisterDataToJson(const std::string& group, int index){
    // インデックスを利用して各キーを生成し、自身のデータを登録
    std::string baseKey = "Enemy_" + std::to_string(index) + "_";

    JsonCoordinator::RegisterItem(group, baseKey + "Name", name_);
    JsonCoordinator::RegisterItem(group, baseKey + "HP", HP_);
    JsonCoordinator::RegisterItem(group, baseKey + "CanEat", canEat_);
    JsonCoordinator::RegisterItem(group, baseKey + "ChasePlayer", cahsePlayer_);
    JsonCoordinator::RegisterItem(group, baseKey + "RoutineName", routineName_);


    // jsonを読み込んだ時に、ルーチン名に対応するポイントを取得するために登録
}

void Enemy::LoadDataFromJson(const std::string& group, int index){
    std::string baseKey = "Enemy_" + std::to_string(index) + "_";

    // HP
    if (auto hpOpt = JsonCoordinator::GetValue(group, baseKey + "HP")){
        HP_ = std::get<int>(*hpOpt);
    }
    // canEat
    if (auto eatOpt = JsonCoordinator::GetValue(group, baseKey + "CanEat")){
        canEat_ = std::get<bool>(*eatOpt);
    }
    // chasePlayer
    if (auto chaseOpt = JsonCoordinator::GetValue(group, baseKey + "ChasePlayer")){
        cahsePlayer_ = std::get<bool>(*chaseOpt);
    }
    // routineName
    if (auto rOpt = JsonCoordinator::GetValue(group, baseKey + "RoutineName")){
        routineName_ = std::get<std::string>(*rOpt);
    }

   
}

void Enemy::SaveData(){
    // グループは「Enemies」で統一
    const std::string groupName = "Enemies";

    JsonCoordinator::SaveGroup(groupName);

}

//////////////////////////////////////////////////////////////////////////
// データのロード
//////////////////////////////////////////////////////////////////////////
void Enemy::LoadData(){}

//////////////////////////////////////////////////////////////////////////
// ダメージ処置
//////////////////////////////////////////////////////////////////////////
void Enemy::Damage(int32_t damage){
    HP_ -= damage;
    isDamaged_ = true;
    if(HP_ <= 0){
        ChangeState(new EnemyState_Down("Enemy_Down",this));
    }
}

//////////////////////////////////////////////////////////////////////////
// コライダー関連
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// その他
//////////////////////////////////////////////////////////////////////////
float Enemy::GetDistanceToPlayer() const{
    if(!pPlayer_){ assert(false); }
    Vector3 playerPos = pPlayer_->GetWorldTranslate();
    Vector3 enemyPos = GetWorldTranslate();
    return MyMath::Length(playerPos - enemyPos);
}

void Enemy::Rename(const std::string& newName){
    SetName(newName);
    JsonCoordinator::RegisterItem(newName, "CanEate", canEat_);
    JsonCoordinator::RegisterItem(newName, "ChasePlayer", cahsePlayer_);
    JsonCoordinator::RegisterItem(newName, "HP", HP_);
    JsonCoordinator::RegisterItem(newName, "routineName", routineName_);
}

//////////////////////////////////////////////////////////////////////////
// 衝突時処理
//////////////////////////////////////////////////////////////////////////


void Enemy::OnCollision(const BaseObject* other,ObjectType objectType){

    other;

    // プレイヤーに攻撃されたら
    if(objectType == ObjectType::PlayerAttack){

        // 無敵時間中はダメージを受けない
        if(unrivalledTime_ > 0.0f){ return; }


        // プレイヤーにダメージを与える
        int32_t preHP = HP_;
        Damage(1);

        if(preHP > (kMaxHP_ / 2) && HP_ <= (kMaxHP_ / 2)){
            // HPが半分以下になったら
            ChangeState(new EnemyState_Down("Enemy_Down",this));
        }

        // 血しぶきを出す
        ParticleManager::AddEffect("blood.json",{0.0f,3.0f,0.0f},GetWorldMatPtr());
        ParticleManager::AddEffect("blood2.json",{0.0f,3.0f,0.0f},GetWorldMatPtr());

        // カメラシェイクを設定する
        SEED::SetCameraShake(0.5f,0.5f,{0.5f,0.5f,0.5f});
    }
}
