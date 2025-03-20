#include "PlayerCorpse.h"

PlayerCorpse::PlayerCorpse(){
    className_ = "PlayerCorpse";
}

PlayerCorpse::~PlayerCorpse(){}

//////////////////////////////////////////////////////////////////////////
// 初期化
//////////////////////////////////////////////////////////////////////////
void PlayerCorpse::Initialize(){
    model_ = std::make_unique<Model>("dinosaur_corpse.obj");
    model_->isParentScale_ = false;
    model_->UpdateMatrix();

    // コライダーエディターの初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_,this);
    InitColliders(ObjectType::PlayerCorpse);
    for(auto& collider : colliders_){
        collider->AddSkipPushBackType(ObjectType::Egg);
        collider->AddSkipPushBackType(ObjectType::Editor);
        collider->AddSkipPushBackType(ObjectType::Enemy);
    }

    SetIsApplyGravity(true);

    weight_ = 4.f;
}

void PlayerCorpse::Initialize(const Vector3& scale){
    Initialize();
    SetScale(scale);
    UpdateMatrix();
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void PlayerCorpse::Update(){
    BaseObject::Update();

    // 地面ではなく,y==0に落下したら削除
    if(this->GetWorldTranslate().y <= -20.0f){
        isAlive_ = false;
    } else{
        HandOverColliders();
    }

}

void PlayerCorpse::OnCollision(BaseObject* other,ObjectType objectType){
    // 移動床に触れている状態
    if((int32_t)objectType & (int32_t)ObjectType::Move){
        // 親子付けを行い移動床基準のトランスフォームに変換
        SetParent(other);

        Vector3 preTranslate = GetWorldTranslate();
        Matrix4x4 invParentMat = InverseMatrix(GetParent()->GetWorldMat());
        Vector3 localTranslate = preTranslate * invParentMat;
        localTranslate *= ExtractScale(GetParent()->GetWorldMat());
        SetTranslate(localTranslate);
        UpdateMatrix();
        Vector3 newTranslate = GetWorldTranslate();
    }
}
