#include "SampleCharacter.h"

// engine

// module
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Source/Manager/CameraManager/CameraManager.h>
#include <SEED/Source/Object/Camera/FollowCamera.h>

// 状態クラスのインクルード
#include "SampleCharacter/SampleCharacterState_Idle.h"
#include "SampleCharacter/SampleCharacterState_Jump.h"
#include "SampleCharacter/SampleCharacterState_Move.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
SampleCharacter::SampleCharacter(): BaseCharacter(){
    className_ = "SampleCharacter";
    name_ = "SampleCharacter";
    Initialize();
}

SampleCharacter::~SampleCharacter(){}

void SampleCharacter::Initialize(){

    // 属性の決定
    objectType_ = ObjectType::Player;

    // モデルの初期化
    model_ = std::make_unique<Model>("Assets/man.gltf");
    model_->UpdateMatrix();
    model_->isRotateWithQuaternion_ = false;
    model_->isParentScale_ = false;

    // コライダーの初期化
    InitColliders(ObjectType::Player);
    for(auto& collider : colliders_){
        collider->AddSkipPushBackType(ObjectType::Egg);
    }

    // ターゲットになる際の注目点のオフセット
    targetOffset_ = Vector3(0.0f,7.0f,0.0f);

    // 状態の初期化
    currentState_ = std::make_unique<SampleCharacterState_Idle>("SampleCharacter_Idle",this);
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void SampleCharacter::Update(){
#ifdef _DEBUG
    ImGui::Begin("SampleCharacter");
    ImGui::End();
#endif // _DEBUG

    BaseCharacter::Update();
}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void SampleCharacter::Draw(){
    BaseCharacter::Draw();
}

//////////////////////////////////////////////////////////////////////////
// フレーム開始時処理
//////////////////////////////////////////////////////////////////////////
void SampleCharacter::BeginFrame(){

    // 落下中かつジャンプ可能でないとき or ジャンプ時
    if((isDrop_ && !IsJumpable()) or isJump_){

        // 親がいれば
        if(GetParent()){
            // 親子付けを解除
            parent_ = nullptr;
            model_->parent_ = nullptr;
            // ワールド基準のトランスフォームに変換
            SetTranslate(GetWorldTranslate());
            SetRotate(GetWorldRotate());
            SetScale(GetWorldScale());
            UpdateMatrix();
        }
    }

    BaseCharacter::BeginFrame();

#ifdef _DEBUG
    ImGui::Begin("Player");
    ImGui::Text("dropSpeed_ : %f",dropSpeed_);
    ImGui::Text("isApplyGravity_ : %d",isApplyGravity_);
    ImGui::Text("isDrop_ : %d",isDrop_);
    ImGui::End();
#endif
}

//////////////////////////////////////////////////////////////////////////
// フレーム終了時処理
//////////////////////////////////////////////////////////////////////////
void SampleCharacter::EndFrame(){
    BaseCharacter::EndFrame();
}


//////////////////////////////////////////////////////////////////////////
// ステート関連
//////////////////////////////////////////////////////////////////////////
void SampleCharacter::HandleMove(const Vector3& acceleration){
    // 移動
    if(isMovable_){
        model_->translate_ += acceleration;
    }

    // 移動制限
    model_->translate_.y = std::clamp(model_->translate_.y,0.0f,10000.0f);
    model_->UpdateMatrix();
}

//////////////////////////////////////////////////////////////////////////
// 基本的な情報をJson形式で出す
//////////////////////////////////////////////////////////////////////////
const nlohmann::json& SampleCharacter::GetJsonData(){
    static nlohmann::json json;

    // BaseObjectの情報と結合して出力
    json.update(BaseCharacter::GetJsonData());
    return json;
}

//////////////////////////////////////////////////////////////////////////
// 衝突時処理
//////////////////////////////////////////////////////////////////////////
void SampleCharacter::OnCollision(BaseObject* other,ObjectType objectType){

    BaseObject::OnCollision(other,objectType);

    // 移動床に触れている状態
    if((int32_t)objectType & (int32_t)ObjectType::Move){
        if(!IsJumpable()){
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
}
