#include "FieldObject_CameraControlArea.h"
#include "StageManager.h"
#include "Scene_Game/Scene_Game.h"
#include  "../GameSystem.h"


////////////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////////////
FieldObject_CameraControlArea::FieldObject_CameraControlArea(){
    // 名前関連の初期化
    className_ = "FieldObject_CameraControlArea";
    name_ = "CameraControlArea";

    // モデルの初期化
    std::string path = "FieldObject/" + name_ + ".obj";
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
    model_->color_.w = 0.2f;
    model_->blendMode_ = BlendMode::ADD;

    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Area);

    // カメラモデルの初期化
    cameraModel_ = std::make_unique<Model>("Assets/Camera.obj");
    cameraModel_->isRotateWithQuaternion_ = false;
    cameraModel_->parent_ = model_.get();
    cameraModel_->isParentScale_ = false;
    FieldObject::Initialize();

    // 全般の初期化
    FieldObject::Initialize();
}

////////////////////////////////////////////////////////////////////////
// 初期化関数
////////////////////////////////////////////////////////////////////////
void FieldObject_CameraControlArea::Initialize(){
}

////////////////////////////////////////////////////////////////////////
// 更新関数
////////////////////////////////////////////////////////////////////////
void FieldObject_CameraControlArea::Update(){

    if(isPositionFixed_){// カメラの位置を固定する場合

        cameraModel_->translate_ = cameraPos_;
        cameraModel_->rotate_ = cameraRotate_;
        cameraMatrix_ = AffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate_, cameraPos_);

    } else{// カメラの位置を固定しない場合

        // offsetをtheta,phi,distanceから計算
        Vector3 offset = MyFunc::CreateVector(theta_, phi_);
        offset *= distance_;

        // カメラモデルの位置を更新
        cameraModel_->translate_ = offset;

        // カメラモデルの回転を更新
        cameraModel_->rotate_ = MyFunc::CalcRotateVec(MyMath::Normalize(offset * -1.0f));
    }

    cameraModel_->Update();
    FieldObject::Update();
}

////////////////////////////////////////////////////////////////////////
// 描画関数
////////////////////////////////////////////////////////////////////////
void FieldObject_CameraControlArea::Draw(){
#ifdef _DEBUG
    model_->color_.w = 0.2f;
    cameraModel_->Draw();
    FieldObject::Draw();
#endif // _DEBUG
}

////////////////////////////////////////////////////////////////////////
// フレーム開始処理
////////////////////////////////////////////////////////////////////////
void FieldObject_CameraControlArea::BeginFrame(){
    FieldObject::BeginFrame();
    preIsCollidePlayer_ = isCollidePlayer_;
    isCollidePlayer_ = false;
}

////////////////////////////////////////////////////////////////////////
// フレーム終了処理
////////////////////////////////////////////////////////////////////////
void FieldObject_CameraControlArea::EndFrame(){
    FieldObject::EndFrame();

    // プレイヤーが衝突しなくなった瞬間の処理
    if(preIsCollidePlayer_ && !isCollidePlayer_){
        if(pCamera_){
            // カメラのターゲットを解除していた場合
            if(isPositionFixed_){
                // 元のターゲットに戻してあげる
                pCamera_->SetTarget(pCamera_->GetPreTarget());
                pCamera_->SetIsInputActive(true);

            } else{
                // 角度と距離を元に戻す
                pCamera_->SetIsInputActive(true);
                pCamera_->SetDistance(preDistance_);
                pCamera_->SetTheta(-3.14f * 0.5f);
                pCamera_->SetPhi(3.14f * 0.45f);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////
// 衝突処理
////////////////////////////////////////////////////////////////////////
void FieldObject_CameraControlArea::OnCollision( BaseObject* other, ObjectType objectType){
    if(objectType == ObjectType::Player){
        isCollidePlayer_ = true;

        // 衝突時
        if(!preIsCollidePlayer_ && isCollidePlayer_){
            Player* player = dynamic_cast<Player*>(other);
            pCamera_ = dynamic_cast<FollowCamera*>(player->GetFollowCamera());

            if(pCamera_){

                // カメラの位置を固定する場合
                if(isPositionFixed_){

                    // カメラのターゲットを解除
                    pCamera_->ReleaseTarget();
                    pCamera_->SetIsInputActive(false);

                    // カメラの位置を固定
                    pCamera_->SetTranslation(cameraModel_->GetWorldTranslate());
                    pCamera_->SetRotation(cameraModel_->GetWorldRotate());

                } else{// カメラの位置を固定しない場合

                    // 角度と距離を固定し、入力を無効化する
                    pCamera_->SetIsInputActive(false);
                    pCamera_->SetTheta(theta_);
                    pCamera_->SetPhi(phi_);
                    preDistance_ = pCamera_->GetDistance();
                    pCamera_->SetDistance(distance_);
                }
            }
        }
    }
}
