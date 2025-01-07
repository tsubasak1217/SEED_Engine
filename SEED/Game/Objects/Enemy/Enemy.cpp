#include "Enemy.h"
#include "EnemyState/EnemyState_Idle.h"
#include "EnemyState/EnemyState_Down.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
Enemy::Enemy(){
    className_ = "Enemy";
    name_ = "Enemy";
    Initialize();
}

Enemy::Enemy(Player* pPlayer){
    className_ = "Enemy";
    name_ = "Enemy";
    pPlayer_ = pPlayer;
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
    currentState_ = std::make_unique<EnemyState_Idle>("Enemy_Idle", this);

    // コライダーの初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, model_->GetWorldMatPtr());
    InitColliders(ObjectType::Enemy);

    // ターゲットになる際の注目点のオフセット
    targetOffset_ = Vector3(0.0f, 3.0f, 0.0f);


    // HP
    kMaxHP_ = 100;
    HP_ = kMaxHP_;
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void Enemy::Update(){
    BaseCharacter::Update();

    // もし無敵時間があれば
    if(unrivalledTime_ > 0.0f){
        unrivalledTime_ -= ClockManager::DeltaTime();
        unrivalledTime_ = std::clamp(unrivalledTime_, 0.0f, 10000.0f);
    }

}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void Enemy::Draw(){
    BaseCharacter::Draw();
}


//////////////////////////////////////////////////////////////////////////
// ダメージ処置
//////////////////////////////////////////////////////////////////////////
void Enemy::Damage(int32_t damage){
    HP_ -= damage;
    isDamaged_ = true;

    if(HP_ <= 0){
        ChangeState(new EnemyState_Down("Enemy_Down", this));
    }
}



//////////////////////////////////////////////////////////////////////////
// コライダー関連
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// その他
//////////////////////////////////////////////////////////////////////////
float Enemy::GetDistanceToPlayer() const{
    if(!pPlayer_){assert(false);}
    return MyMath::Length(GetWorldTranslate(), pPlayer_->GetWorldTranslate());
}


//////////////////////////////////////////////////////////////////////////
// 衝突時処理
//////////////////////////////////////////////////////////////////////////

void Enemy::OnCollision(const BaseObject* other, ObjectType objectType){

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
            ChangeState(new EnemyState_Down("Enemy_Down", this));
        }
        
    }
}
