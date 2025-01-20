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
    model_ = std::make_unique<Model>("Assets/zombie.gltf");
    model_->UpdateMatrix();
    model_->isRotateWithQuaternion_ = false;

    // 状態の初期化
    currentState_ = std::make_unique<EnemyState_RoutineMove>("Enemy_Idle",this);

    // コライダーの初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_,this);
    InitColliders(ObjectType::Enemy);

    // ターゲットになる際の注目点のオフセット
    targetOffset_ = Vector3(0.0f,3.0f,0.0f);

    // HP
    kMaxHP_ = 100;
    HP_ = kMaxHP_;

    //! TODO : ユニーク ID から 読み込む
    JsonCoordinator::RegisterItem(name_,"CanEate",canEat_);
    JsonCoordinator::RegisterItem(name_,"ChasePlayer",cahsePlayer_);
    JsonCoordinator::RegisterItem(name_, "HP", HP_);
    // ! TODO : Json で 対応する RootionePoints nameを 保存,読み込み
    JsonCoordinator::RegisterItem(name_, "routineName", routineName_);
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void Enemy::Update(){
    BaseCharacter::Update();

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
// ImGui
//////////////////////////////////////////////////////////////////////////
void Enemy::ShowImGui(){
    JsonCoordinator::RenderAdjustableItem(name_, "CanEate");
    JsonCoordinator::RenderAdjustableItem(name_, "ChasePlayer");
    JsonCoordinator::RenderAdjustableItem(name_, "HP");

    const EnemyManager* manager = GetManager(); // 敵が所属するマネージャーを取得するメソッド

    std::vector<std::string> routineNames = manager->GetRoutineNames();

    // 2. 現在のルーチン名を取得し、インデックスを特定
    std::string currentRoutine = GetRoutineName();
    int currentIndex = 0;
    for (size_t i = 0; i < routineNames.size(); ++i){
        if (routineNames[i] == currentRoutine){
            currentIndex = static_cast< int >(i);
            break;
        }
    }

    // 3. ImGui Combo 用にルーチン名の C文字列配列を作成
    std::vector<const char*> routineNamesCStr;
    routineNamesCStr.reserve(routineNames.size());
    for (const auto& name : routineNames){
        routineNamesCStr.push_back(name.c_str());
    }

    // 4. Combo ウィジェットの表示と選択処理
    if (ImGui::Combo("Select Routine", &currentIndex, routineNamesCStr.data(), static_cast< int >(routineNamesCStr.size()))){
        // 選択が変更された場合の処理
        const std::string& selectedRoutine = routineNames[currentIndex];
        routineName_ = selectedRoutine;

        // ルーチン変更に基づく処理
         routinePoints = manager->GetRoutinePoints(selectedRoutine);
    }
}

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
    return MyMath::Length(GetWorldTranslate(),pPlayer_->GetWorldTranslate());
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
