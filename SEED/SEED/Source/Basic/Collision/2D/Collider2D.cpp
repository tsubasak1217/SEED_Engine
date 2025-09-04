#include "Collider2D.h"
#include <SEED/Source/SEED.h>
#include <SEED/Source/Basic/Object/GameObject.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//    static変数
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t Collider2D::nextID_ = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     初期化・終了関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


Collider2D::Collider2D(){
    colliderID_ = nextID_++;
}

Collider2D::~Collider2D(){}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     更新関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Collider2D::Update(){
    // 行列の更新
    UpdateMatrix();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     行列更新関数
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Collider2D::UpdateMatrix(){

    // ローカル行列の更新
    localMat_ = AffineMatrix(scale_, rotate_, translate_);

    // ワールド行列の更新
    if(parentMat_){

        if(isParentRotate_ + isParentScale_ + isParentTranslate_ == 3){
            worldMat_ = localMat_  * (*parentMat_);
            return;
        } else{
            
            Matrix3x3 cancelMat = IdentityMat3();

            // 親の行列から取り出した要素を打ち消す行列を作成
            if(!isParentScale_){
                Vector2 inverseScale = Vector2(1.0f,1.0f)/ExtractScale(*parentMat_);
                cancelMat = cancelMat * ScaleMatrix(inverseScale);
            }

            if(!isParentRotate_){
                float inverseRotate = ExtractRotation(*parentMat_) * -1.0f;
                cancelMat = cancelMat * RotateMatrix(inverseRotate);
            }

            if(!isParentTranslate_){
                Vector2 inverseTranslate = ExtractTranslation(*parentMat_) * -1.0f;
                cancelMat = cancelMat * TranslateMatrix(inverseTranslate);
            }

            worldMat_ = (*parentMat_ * cancelMat)* localMat_;

        }

    } else{
        worldMat_ = localMat_;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     描画関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Collider2D::Draw(){}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     フレーム開始時関数
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Collider2D::BeginFrame(){

    // 衝突フラグの更新
    preIsCollision_ = isCollision_;
    isCollision_ = false;

    // 衝突リストのクリア
    collisionList_.clear();

    // 色の初期化
    color_ = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     衝突判定関数
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Collider2D::CheckCollision(Collider2D* collider){ collider; }

bool Collider2D::CheckCollision(const Quad2D& quad){
    quad;
    return false;
}

bool Collider2D::CheckCollision(const Circle& circle){
    circle;
    return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     衝突時関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Collider2D::OnCollision(Collider2D* collider,ObjectType objectType){

    // 当たり判定をスキップするオブジェクトなら衝突判定を行わない
    if(CanSkipPushBack(objectType)){ return; }

    // 衝突フラグを立てる
    isCollision_ = true;
    // 赤色に変更
    color_ = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    // 親オブジェクトにも衝突を通知
    if(ownerObject_){
        ownerObject_->OnCollision(collider->ownerObject_);
    }

    // 衝突リストに追加
    collisionList_.insert(collider->colliderID_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     衝突判定用のAABB更新関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Collider2D::UpdateBox(){}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     押し戻ししないオブジェクトの確認・追加・削除関数
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Collider2D::CanSkipPushBack(ObjectType objectType){
    if(skipPushBackTypes_.find(objectType) != skipPushBackTypes_.end()){
        return true;
    }

    return false;
}

void Collider2D::AddSkipPushBackType(ObjectType objectType){
    if(skipPushBackTypes_.find(objectType) == skipPushBackTypes_.end()){
        skipPushBackTypes_.insert(objectType);
    }
}

void Collider2D::RemoveSkipPushBackType(ObjectType objectType){
    if(skipPushBackTypes_.find(objectType) != skipPushBackTypes_.end()){
        skipPushBackTypes_.erase(objectType);
    }
}

void Collider2D::DiscardPreCollider(){}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     衝突時の押し戻し関数
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Collider2D::PushBack(Collider2D* collider1, Collider2D* collider2, CollisionData2D collisionData){

    // どちらかがすり抜け可能なら押し戻しを行わない
    if(collider1->isGhost_ or collider2->isGhost_){ return; }
    // どちらかが押し戻しをスキップするオブジェクトなら押し戻しを行わない
    if(collider1->CanSkipPushBack(collider2->GetObjectType()) or collider2->CanSkipPushBack(collider1->GetObjectType())){ return; }

    GameObject2D* parent[2] = { collider1->GetOwnerObject(),collider2->GetOwnerObject() };
    Vector2 pushBack = collisionData.hitNormal.value() * collisionData.collideDepth.value();

    /*=========================== Collider1の押し戻し =============================*/
    if(parent[0]){
        // 法線 * 押し戻す割合
        parent[0]->AddWorldTranslate(
            pushBack * collisionData.pushBackRatio_A.value()
        );

        // ある程度平らな面に衝突した場合は落下フラグをオフにする
        if(MyMath::Dot(collisionData.hitNormal.value(), { 0.0f,-1.0f }) > 0.7f){
            collider1->ownerObject_->SetIsOnGround(true);
        
        }
        // 天井判定
        else if(MyMath::Dot(collisionData.hitNormal.value(), { 0.0f,1.0f }) > 0.7f){
            collider1->ownerObject_->SetIsCeiling(true);
        }

        // 親の行列を更新する
        parent[0]->UpdateMatrix();

    } else{
        collider1->AddTranslate(pushBack * collisionData.pushBackRatio_A.value());
    }

    /*=========================== Collider2の押し戻し =============================*/
    if(parent[1]){
        parent[1]->AddWorldTranslate(
            -pushBack * collisionData.pushBackRatio_B.value()
        );

        // ある程度平らな面に衝突した場合は落下フラグをオフにする
        if(MyMath::Dot(-collisionData.hitNormal.value(), { 0.0f,-1.0f }) > 0.7f){
            collider2->ownerObject_->SetIsOnGround(true);
        }
        // 天井判定
        else if(MyMath::Dot(-collisionData.hitNormal.value(), { 0.0f,1.0f }) > 0.7f){
            collider2->ownerObject_->SetIsCeiling(true);
        }

        // 親の行列を更新する
        parent[1]->UpdateMatrix();

    } else{
        collider2->AddTranslate(-pushBack * collisionData.pushBackRatio_B.value());
    }

    /*============================= 行列を更新して終了 ==============================*/
    collider1->UpdateMatrix();
    collider2->UpdateMatrix();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     ImGuiでの編集関数
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Collider2D::Edit(){
#ifdef _DEBUG

    std::string colliderID = "##" + std::to_string(colliderID_);// コライダーID
    ImGui::Text("------ physics ------");
    ImGui::InputFloat(std::string("Mass" + colliderID).c_str(), &mass_);
    ImGui::InputFloat(std::string("Miu" + colliderID).c_str(), &miu_);
    ImGui::Checkbox(std::string("IsMovable" + colliderID).c_str(), &isMovable_);
    ImGui::Checkbox(std::string("IsGhost" + colliderID).c_str(), &isGhost_);

#endif // _DEBUG
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     jsonデータ取得関数
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
nlohmann::json Collider2D::GetJsonData(){
    nlohmann::json j;

    j["isMovable"] = isMovable_;
    j["mass"] = mass_;
    j["miu"] = miu_;
    j["isGhost"] = isGhost_;
    j["isParentRotate"] = isParentRotate_;
    j["isParentScale"] = isParentScale_;
    j["isParentTranslate"] = isParentTranslate_;

    return j;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     jsonデータ読み込み関数
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Collider2D::LoadFromJson(const nlohmann::json& jsonData){

    // 全般情報の読み込み
    isMovable_ = jsonData["isMovable"];
    mass_ = jsonData["mass"];
    miu_ = jsonData["miu"];
    isGhost_ = jsonData["isGhost"];
    isParentRotate_ = jsonData["isParentRotate"];
    isParentScale_ = jsonData["isParentScale"];
    isParentTranslate_ = jsonData["isParentTranslate"];
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     このフレームに動いたかどうか
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Collider2D::IsMoved(){ return false; }
