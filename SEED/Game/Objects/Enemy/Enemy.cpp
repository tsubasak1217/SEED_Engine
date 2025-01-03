#include "Enemy.h"
#include "EnemyState/EnemyState_Idle.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
Enemy::Enemy(){
    name_ = "Enemy";
    Initialize();
}

Enemy::Enemy(Player* pPlayer){
    name_ = "Enemy";
    pPlayer_ = pPlayer;
    Initialize();
}

Enemy::~Enemy(){}

void Enemy::Initialize(){
    // モデルの初期化
    model_ = std::make_unique<Model>("Assets/zombie.gltf");
    model_->UpdateMatrix();
    model_->isRotateWithQuaternion_ = false;

    // コライダーの初期化
    ResetCollider();
    InitCollider();

    // ターゲットになる際の注目点のオフセット
    targetOffset_ = Vector3(0.0f, 3.0f, 0.0f);

    // 状態の初期化
    currentState_ = std::make_unique<EnemyState_Idle>(this);
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void Enemy::Update(){
    BaseCharacter::Update();
}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void Enemy::Draw(){
    BaseCharacter::Draw();
}

//////////////////////////////////////////////////////////////////////////
// コライダー関連
//////////////////////////////////////////////////////////////////////////
void Enemy::InitCollider(){
    colliders_.emplace_back(new Collider_OBB());
    Collider_OBB* obb = dynamic_cast<Collider_OBB*>(colliders_.back().get());
    obb->SetParentObject(this);
    obb->SetParentMatrix(model_->GetWorldMatPtr());
    obb->SetSize({ 3.0f,6.0f,3.0f });
    obb->offset_ = { 0.0f, 3.0f, 0.0f };
}

//////////////////////////////////////////////////////////////////////////
// その他
//////////////////////////////////////////////////////////////////////////
float Enemy::GetDistanceToPlayer() const{
    if(!pPlayer_){assert(false);}
    return MyMath::Length(GetWorldTranslate(), pPlayer_->GetWorldTranslate());
}
