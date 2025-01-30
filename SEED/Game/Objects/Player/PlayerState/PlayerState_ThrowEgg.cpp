#include "PlayerState_ThrowEgg.h"

// Others Behavior
#include "PlayerState_Idle.h"
//eggState
#include "Egg/State/EggState_Thrown.h"
#include "Egg/State/EggState_Follow.h"
#include "Egg/State/EggState_Idle.h"
// stl
#include <algorithm>

// Engine
#include "SEED.h"
#include "ClockManager.h"
#include "InputManager.h"

// Object
#include "../Player/Player.h"
#include "Egg/Egg.h"
//lib
#include "../adapter/json/JsonCoordinator.h"
#include "../PlayerInput/PlayerInput.h"

// math
#include "Matrix4x4.h"
#include "MatrixFunc.h"
#include "MyMath.h"
#include "MyFunc.h"

PlayerState_ThrowEgg::PlayerState_ThrowEgg(const std::string& stateName, BaseCharacter* player){
    Initialize(stateName, player);
}

PlayerState_ThrowEgg::~PlayerState_ThrowEgg(){}

void PlayerState_ThrowEgg::Initialize(const std::string& stateName, BaseCharacter* character){
    ICharacterState::Initialize(stateName, character);

    JsonCoordinator::RegisterItem("Player", "eggOffset", eggOffset_);
    JsonCoordinator::RegisterItem("Player", "throwPower", throwPower_);
    JsonCoordinator::RegisterItem("Player", "throwDirection", throwDirection_);
    JsonCoordinator::RegisterItem("Player", "pressForcus", pressForcus_);

    // EggManagerを取得するために Player をダウンキャスト
    Player* pPlayer = dynamic_cast<Player*>(pCharacter_);
    if(!pPlayer){
        assert(false);
    }
    // EggManagerを取得
    eggManager_ = pPlayer->GetEggManager();
    // 投げる卵を取得
    throwEgg_ = eggManager_->GetFrontEgg().get();

    throwEgg_->ChangeState(new EggState_Idle(throwEgg_));
    // 投げる卵の重さを取得
    eggWeight_ = dynamic_cast<Egg*>(throwEgg_)->GetWeight();
    // 狙う関連
    aimRadius_ = 25.0f;
    aimPos_ = Vector3(0.0f, 0.0f, 0.0f);
    aimModel_ = std::make_unique<Model>("Egg.obj");
    aimModel_->color_ = { 1.0f,1.0f,0.0f,0.3f };
    throwSimulationEgg_ = std::make_unique<BaseObject>("Egg.obj");
    throwSimulationEgg_->InitColliders("Egg.json", ObjectType::Egg);
    throwSimulationEgg_->AddSkipPushBackType(ObjectType::Egg);
    throwSimulationEgg_->AddSkipPushBackType(ObjectType::Player);

    //Playerの Animation を aim に変更
    pPlayer->SetAnimation("aim", false);
}

void PlayerState_ThrowEgg::Update(){

    // 移動状態の更新 & アニメーションの変更
    UpdateMovingState();
    ChangeAnimation();

    // 狙う
    Aim();
    SimulateThrowEgg();

#ifdef _DEBUG
    // throwDirection_の値を取得
    auto throwDirectionOpt = JsonCoordinator::GetValue("Player", "throwDirection");
    if(throwDirectionOpt.has_value()){
        throwDirection_ = std::get<Vector3>(*throwDirectionOpt);
    }
    throwDirection_ = MyMath::Normalize(throwDirection_);
    // 正規化した値を セット
    JsonCoordinator::SetValue("Player", "throwDirection", throwDirection_);
#endif // _DEBUG


    // 卵 の 位置 を 更新
    throwEgg_->SetTranslate(pCharacter_->GetWorldTranslate() + (eggOffset_ * RotateYMatrix(pCharacter_->GetWorldRotate().y)));
    throwEgg_->HandleRotate(pCharacter_->GetWorldRotate());

}

constexpr float timePerSegment = 0.1f / 32.0f;
void PlayerState_ThrowEgg::Draw(){

    controlPoints_[0] = throwEgg_->GetWorldTranslate();
    controlPoints_[2] = aimModel_->GetWorldTranslate();
    Vector3 dif = controlPoints_[2] - controlPoints_[0];// 卵から狙うモデルへのベクトル
    float length = MyMath::Length(dif);// 卵から狙うモデルまでの距離
    float leftLength = aimRadius_ - length;// 残りの長さ(高さに変換する分)
    controlPoints_[1] = (controlPoints_[0] + dif * 0.5f) + Vector3(0.0f, leftLength, 0.0f);

    // ベジェ曲線の描画
    SEED::DrawBezier(controlPoints_[0], controlPoints_[1], controlPoints_[2], 32, { 0.3f,1.0f,0.3f,1.0f });

    // 狙うモデルの描画
    aimModel_->Draw();
    throwSimulationEgg_->Draw();
}

// コライダーを渡す
void PlayerState_ThrowEgg::HandOverColliders(){
    ICharacterState::HandOverColliders();
    // 卵のコライダーを渡す
    throwEgg_->HandOverColliders();
}

void PlayerState_ThrowEgg::ManageState(){
    // キャンセル
    if(PlayerInput::CharacterMove::CancelFocusEgg(pressForcus_)){
        throwEgg_->ChangeState(new EggState_Follow(throwEgg_, pCharacter_));
        pCharacter_->ChangeState(new PlayerState_Idle("Player_Idle", pCharacter_));
        return;
    }

    // 投げる
    if(PlayerInput::CharacterMove::ThrowEggOnFocus(pressForcus_)){
        throwEgg_->ChangeState(new EggState_Thrown(throwEgg_, controlPoints_[0], controlPoints_[1], controlPoints_[2] ));

        //Playerの Animation を throw に変更
        pCharacter_->SetAnimation("throw", false);
        pCharacter_->ChangeState(new PlayerState_Idle("Player_Idle", pCharacter_));
        return;
    }
}

void PlayerState_ThrowEgg::UpdateMovingState(){
    preIsMoving_ = isMoving_;
    if(MyMath::LengthSq(pCharacter_->GetPrePos() - pCharacter_->GetWorldTranslate()) >= 0.0001f){
        isMoving_ = true;
    } else{
        isMoving_ = false;
    }
}

void PlayerState_ThrowEgg::ChangeAnimation(){
    if(!isFirstAnimationEnd_){
        isFirstAnimationEnd_ = pCharacter_->GetIsEndAnimation();
        if(isFirstAnimationEnd_){
            pCharacter_->SetAnimation("handUpIdle", true);
        }
    }

    // 移動状態が変わったら
    if(preIsMoving_ == isMoving_){
        return;
    }

    // 動いているなら
    if(isMoving_){
        isFirstAnimationEnd_ = true;
        pCharacter_->SetAnimation("handUpRunning", true);
    } else{
        isFirstAnimationEnd_ = true;
        pCharacter_->SetAnimation("handUpIdle", true);
    }
}

// 狙う
void PlayerState_ThrowEgg::Aim(){
    static float targetMoveSpeed = 24.0f;

    if(PlayerInput::CharacterMove::Aim() != Vector2(0.0f, 0.0f)){
        Vector2 moveVec = PlayerInput::CharacterMove::Aim();

        // 視点の回転を加味して今の視点でXZ平面を動かせるよう回転
        Matrix3x3 rotateMat = InverseMatrix(RotateMatrix(SEED::GetCamera()->GetRotation().y));
        moveVec *= rotateMat;

        // 移動
        aimPos_ += Vector3(moveVec.x, 0.0f, moveVec.y) * targetMoveSpeed * ClockManager::DeltaTime();

        // 範囲内に収める
        if(MyMath::Length(aimPos_) > aimRadius_){
            aimPos_ = MyMath::Normalize(aimPos_) * aimRadius_;
        }

        // キャラクターの向きを狙う方向に向ける
        Vector3 aimDirection = MyMath::Normalize(aimPos_);
        Vector3 rotation = MyFunc::CalcRotateVec(aimDirection);
        pCharacter_->SetRotate(rotation);

    }


    // 狙うモデルの位置を更新
    aimModel_->translate_ = pCharacter_->GetWorldTranslate() + aimPos_;
    aimModel_->UpdateMatrix();

    ImGui::Begin("Aim");
    ImGui::Text("aimPos_ : %f %f %f", aimPos_.x, aimPos_.y, aimPos_.z);
    ImGui::Text("charaTrans : %f %f %f", pCharacter_->GetWorldTranslate().x, pCharacter_->GetWorldTranslate().y, pCharacter_->GetWorldTranslate().z);
    ImGui::Text("aimModelTrans : %f %f %f", aimModel_->translate_.x, aimModel_->translate_.y, aimModel_->translate_.z);
    ImGui::DragFloat("aimSpeed", &targetMoveSpeed, 0.1f, 0.0f, 100.0f);
    ImGui::End();
}

void PlayerState_ThrowEgg::SimulateThrowEgg(){

    static float kSimulateTime = 1.0f;

    // 時間の更新
    simulateTime_ += ClockManager::DeltaTime();
    float t = simulateTime_ / kSimulateTime;

    // シミュレーション卵の位置を更新
    throwSimulationEgg_->SetTranslate(MyMath::Bezier(controlPoints_[0], controlPoints_[1], controlPoints_[2], t));
    throwSimulationEgg_->UpdateMatrix();

    // 当たったら時間をリセット
    if(throwSimulationEgg_->GetIsCollide()){
        simulateTime_ = 0.0f;
    }

    // 結構通り過ぎたらリセット
    if(t > 2.0f){
        simulateTime_ = 0.0f;
    }
}

/*
  std::string unti = "unti by aoi";
*/