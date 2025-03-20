#include "FieldObject_CameraControlArea.h"
#include "StageManager.h"
#include "Scene_Game/Scene_Game.h"
#include  "../GameSystem.h"

////////////////////////////////////////////////////////////////////////
// 静的メンバ変数
////////////////////////////////////////////////////////////////////////
bool FieldObject_CameraControlArea::isCollidePlayerAll_ = false;

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
    cameraModel_ = std::make_unique<BaseObject>("Assets/Camera.obj");
    cameraModel_->SetRotateWithQuaternion(false);
    cameraModel_->SetParent(model_.get());
    cameraModel_->SetIsParentScale(false);
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

        cameraModel_->SetTranslate(cameraPos_);
        cameraModel_->SetRotate(cameraRotate_);
        cameraMatrix_ = AffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate_, cameraPos_);

    } else{// カメラの位置を固定しない場合

        // offsetをtheta,phi,distanceから計算
        Vector3 offset = MyFunc::CreateVector(theta_, phi_);
        offset *= distance_;

        // カメラモデルの位置を更新
        cameraModel_->SetTranslate(offset);

        // カメラモデルの回転を更新
        cameraModel_->SetRotate(MyFunc::CalcRotateVec(MyMath::Normalize(offset * -1.0f)));
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
    // offsetをtheta,phi,distanceから計算
    Vector3 offset = MyFunc::CreateVector(exitTheta_, exitPhi_);
    offset *= distance_;
    cameraModel_->SetTranslate(offset);
    cameraModel_->SetRotate(MyFunc::CalcRotateVec(MyMath::Normalize(offset * -1.0f)));
    cameraModel_->SetColor(Vector4(1.0f, 1.0f, 0.0f, 1.0f));
    cameraModel_->UpdateMatrix();
    // もう一度描画
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
    isCollidePlayerAll_ = false;
}

////////////////////////////////////////////////////////////////////////
// フレーム終了処理
////////////////////////////////////////////////////////////////////////
void FieldObject_CameraControlArea::EndFrame(){
    FieldObject::EndFrame();

    // プレイヤーが衝突しなくなった瞬間の処理
    if(preIsCollidePlayer_ && !isCollidePlayer_){
        Player* player = dynamic_cast<Player*>(pCamera_->GetTarget());

        if(pCamera_){
            // カメラのターゲットを解除していた場合
            if(isPositionFixed_){
                // 元のターゲットに戻してあげる
                if(player){
                    pCamera_->SetTarget(player);
                }
            }

            if(!isCollidePlayerAll_){
                // 角度と距離を元に戻す
                pCamera_->SetIsInputActive(true);
                pCamera_->SetDistance(50.0f);

                if(isOutControl_){
                    pCamera_->SetTheta(exitTheta_);
                    pCamera_->SetPhi(exitPhi_);
                }
                // 空にしておく
                preDistance_ = std::nullopt;
            }

            if(player){
                player->SetPreCameraRotate(pCamera_->GetRotation());
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////
// 衝突処理
////////////////////////////////////////////////////////////////////////
void FieldObject_CameraControlArea::OnCollision(BaseObject* other, ObjectType objectType){
    if(objectType == ObjectType::Player){
        isCollidePlayer_ = true;
        isCollidePlayerAll_ = true;

        // 衝突時
        if(!preIsCollidePlayer_ && isCollidePlayer_){
            Player* player = dynamic_cast<Player*>(other);
            pCamera_ = dynamic_cast<FollowCamera*>(player->GetFollowCamera());

            // 角度が急に変わってもスティックを離すまでは同じベクトルで移動させるための保存
            if(player->GetPreCameraRotate() == std::nullopt){
                if(isSavePreCameraRotate_){
                    player->SetPreCameraRotate(pCamera_->GetRotation());
                }
            }

            if(pCamera_){

                // 元々のカメラ距離を記憶
                if(preDistance_ == std::nullopt){
                    preDistance_ = pCamera_->GetDistance();
                }

                // カメラの位置を固定する場合
                if(isPositionFixed_){

                    // カメラのターゲットを解除
                    pCamera_->SetTarget(cameraModel_.get());
                    pCamera_->SetIsInputActive(false);

                    // カメラの位置を固定
                    pCamera_->SetTheta(3.14f * 0.5f);
                    pCamera_->SetPhi(3.14f * 0.5f);
                    pCamera_->SetDistance(0.05f);

                } else{// カメラの位置を固定しない場合

                    if(pCamera_->GetTarget() != player){
                        pCamera_->SetTarget(player);
                    }

                    // 角度と距離を固定し、入力を無効化する
                    pCamera_->SetIsInputActive(false);
                    pCamera_->SetTheta(theta_);
                    pCamera_->SetPhi(phi_);
                    pCamera_->SetDistance(distance_);
                }
            }

            if(isOnceEvent_){
                // 一度触れたら消える場合
                removeFlag_ = true;
                pCamera_->SetIsInputActive(true);
            }

        }

        // phiが変わったら戻す
        //if(pCamera_->GetPhi() != phi_){
        //    pCamera_->SetPhi(phi_);
        //}
    }
}
