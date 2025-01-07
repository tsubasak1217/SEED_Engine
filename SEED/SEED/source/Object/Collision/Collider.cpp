#include "Collider.h"
#include "SEED.h"
#include "Base/BaseObject.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//    static変数
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t Collider::nextID_ = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     初期化・終了関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


Collider::Collider(){
    colliderID_ = nextID_++;
}

Collider::~Collider(){}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     更新関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Collider::Update(){

    // 衝突フラグの更新
    preIsCollision_ = isCollision_;
    isCollision_ = false;

    // 衝突リストのクリア
    collisionList_.clear();

    // 色の初期化
    color_ = Vector4(0.0f, 0.0f, 1.0f, 1.0f);

    // コライダーのアニメーション時間の更新
    if(animationData_){
        animationTime_ += ClockManager::DeltaTime();
        if(isLoop_ or isEditorLoop_){

            // エディターじゃない場合かつループしない場合は最後のフレームで止める
            if(!isLoop_ && isEdit_ == false){
                animationTime_ = std::clamp(animationTime_, 0.0f, animationData_->GetDuration());
            } else{
                // ループする
                if(animationData_->GetDuration() != 0.0f){
                    animationTime_ = std::fmod(animationTime_, animationData_->GetDuration());
                } else{
                    animationTime_ = 0.0f;// 0除算対策
                }
            }

        } else{// ループしない
            animationTime_ = std::clamp(animationTime_, 0.0f, animationData_->GetDuration());
        }

        // アニメーションデータの取得
        scale_ = animationData_->GetScale(animationTime_);
        rotate_ = Quaternion::ToEuler(animationData_->GetRotation(animationTime_));
        translate_ = animationData_->GetTranslation(animationTime_);
    }

    // 行列の更新
    UpdateMatrix();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     行列更新関数
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Collider::UpdateMatrix(){

    // アニメーションが存在する場合には各トランスフォームを計算
    if(animationData_){
        scale_ = animationData_->GetScale(animationTime_);
        rotate_ = Quaternion::ToEuler(animationData_->GetRotation(animationTime_));
        translate_ = animationData_->GetTranslation(animationTime_);
    }

    // ローカル行列の更新
    localMat_ = AffineMatrix(scale_, rotate_, translate_);

    // ワールド行列の更新
    if(parentMat_){

        if(isParentRotate_ + isParentScale_ + isParentTranslate_ == 3){
            worldMat_ = localMat_  * (*parentMat_);
            return;
        } else{
            
            Matrix4x4 cancelMat = IdentityMat4();

            // 親の行列から取り出した要素を打ち消す行列を作成
            if(!isParentScale_){
                Vector3 inverseScale = Vector3(1.0f,1.0f,1.0f)/ExtractScale(*parentMat_);
                cancelMat = cancelMat * ScaleMatrix(inverseScale);
            }

            if(!isParentRotate_){
                Vector3 inverseRotate = ExtractRotation(*parentMat_) * -1.0f;
                cancelMat = cancelMat * RotateMatrix(inverseRotate);
            }

            if(!isParentTranslate_){
                Vector3 inverseTranslate = ExtractTranslation(*parentMat_) * -1.0f;
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

void Collider::Draw(){}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     衝突時関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Collider::CheckCollision(Collider* collider){ collider; }

void Collider::OnCollision(Collider* collider){

    // 衝突フラグを立てる
    isCollision_ = true;
    // 赤色に変更
    color_ = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    // 親オブジェクトにも衝突を通知
    if(parentObject_){
        parentObject_->OnCollision(collider->parentObject_);
    }

    // 衝突リストに追加
    collisionList_.insert(collider->colliderID_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     衝突判定用のAABB更新関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Collider::UpdateBox(){}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     ImGuiでの編集関数
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Collider::Edit(){}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     jsonデータ取得関数
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
nlohmann::json Collider::GetJsonData(){
    nlohmann::json j;

    j["isMovable"] = isMovable_;
    j["mass"] = mass_;
    j["miu"] = miu_;
    j["isParentRotate"] = isParentRotate_;
    j["isParentScale"] = isParentScale_;
    j["isParentTranslate"] = isParentTranslate_;
    j["isAnimation"] = isAnimation_;

    if(isAnimation_){
        if(animationData_){
            j["isLoop"] = isLoop_;
            j["animationData"] = animationData_->GetJsonData();
        }
    }

    return j;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     jsonデータ読み込み関数
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Collider::LoadFromJson(const nlohmann::json& jsonData){

    // 全般情報の読み込み
    isMovable_ = jsonData["isMovable"];
    mass_ = jsonData["mass"];
    miu_ = jsonData["miu"];
    isParentRotate_ = jsonData["isParentRotate"];
    isParentScale_ = jsonData["isParentScale"];
    isParentTranslate_ = jsonData["isParentTranslate"];
    isAnimation_ = jsonData["isAnimation"];

    // アニメーションデータがある場合はインスタンスを作成し読み込む
    if(isAnimation_){
        isLoop_ = jsonData["isLoop"];
        animationData_ = ColliderAnimationData();
        animationData_->SetParentMat(parentMat_);
        animationData_->LoadFromJson(jsonData["animationData"],parentMat_);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     アニメーション編集関数
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Collider::EditAnimation(){
    // アニメーションするかどうか
    if(isAnimation_){

        // アニメーションデータがない場合は作成
        if(animationData_ == std::nullopt){
            animationData_ = ColliderAnimationData();
            animationData_->SetParentMat(parentMat_);
        }

        // Editorでループさせるかどうか
        ImGui::Checkbox("Loop", &isEditorLoop_);

        // アニメーションデータの編集
        std::string animationName = "AnimationData( collider_" + std::to_string(colliderID_) + " )";
        animationData_->Edit(animationName.c_str());

    }
}
