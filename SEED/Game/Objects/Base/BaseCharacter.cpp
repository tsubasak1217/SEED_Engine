#include "BaseCharacter.h"
#include "InputManager.h"
#include "ImGuiManager.h"
#include "ICharacterState.h"
#include "TextureManager/TextureManager.h"
#include "SEED.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
BaseCharacter::BaseCharacter(): BaseObject(){
    name_ = "unnamed";
    Initialize();
}

BaseCharacter::~BaseCharacter(){}

void BaseCharacter::Initialize(){
    // モデルの初期化
    model_ = std::make_unique<Model>("Assets/man.gltf");
    model_->UpdateMatrix();
    model_->isRotateWithQuaternion_ = false;
}


//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void BaseCharacter::Update(){

    // 前フレームの座標の保存
    prePos_ = GetWorldTranslate();

    // フラグの初期化
    isDamaged_ = false;

    // 状態に応じた更新処理
    if(currentState_){
        // 更新処理
        currentState_->Update();
    }

    //ジャンプ
    Jump();

    BaseObject::Update();
}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void BaseCharacter::Draw(){

    // 状態に応じた描画処理
    if(currentState_){
        currentState_->Draw();
    }

    // 基本モデルの描画
    BaseObject::Draw();

    Quad q = Quad(
        Vector3(-3.0f, 0.01f, 3.0f),
        Vector3(3.0f, 0.01f, 3.0f),
        Vector3(-3.0f, 0.01f, -3.0f),
        Vector3(3.0f, 0.01f, -3.0f)
    );

    q.GH = TextureManager::LoadTexture("ParticleTextures/particle.png");
    q.blendMode = BlendMode::SUBTRACT;

    for(int i = 0; i < 4; i++){
        q.localVertex[i] += GetWorldTranslate();
        q.localVertex[i].y = 0.01f;
    }

    SEED::DrawQuad(q);
}

//////////////////////////////////////////////////////////////////////////
// フレーム開始時の処理
//////////////////////////////////////////////////////////////////////////

void BaseCharacter::BeginFrame(){
    BaseObject::BeginFrame();
}

//////////////////////////////////////////////////////////////////////////
// フレーム終了時の処理
//////////////////////////////////////////////////////////////////////////
void BaseCharacter::EndFrame(){

    // 基本クラスの終了処理
    BaseObject::EndFrame();

    // ジャンプフラグの管理
    EndFrameJumpFlagUpdate();

    // 状態に応じた終了処理
    if(currentState_){
        currentState_->EndFrame();
    }

}

//////////////////////////////////////////////////////////////////////////
// フレーム終了時の落下関連の更新処理
//////////////////////////////////////////////////////////////////////////
void BaseCharacter::EndFrameJumpFlagUpdate(){
    // 落下フラグが降りたらジャンプフラグも降ろす
    if(!isDrop_){
        isJump_ = false;
    }
}



//////////////////////////////////////////////////////////////////////////
// ステート関連
//////////////////////////////////////////////////////////////////////////
void BaseCharacter::ChangeState(ICharacterState* nextState){
    currentState_.reset(nextState);
}

void BaseCharacter::HandleMove(const Vector3& acceleration){
    // 移動
    model_->translate_ += acceleration;
}

void BaseCharacter::HandleRotate(const Vector3& rotate){
    // 回転
    model_->rotate_ = rotate;
    model_->UpdateMatrix();
}

//////////////////////////////////////////////////////////////////////////
// 衝突判定関連
//////////////////////////////////////////////////////////////////////////
void BaseCharacter::HandOverColliders(){

    // 基本コライダーを渡す
    BaseObject::HandOverColliders();

    // state固有のコライダーを渡す
    if(currentState_){
        currentState_->HandOverColliders();
    }
}

// コライダーの初期化
void BaseCharacter::InitColliders(ObjectType objectType){
    BaseObject::InitColliders(objectType);
    colliders_;
    // state固有のコライダーを初期化
    if(currentState_){
        currentState_->InitColliders(objectType);
    }
}

// ダメージを受ける処理
void BaseCharacter::Damage(int32_t damage){
    HP_ -= damage;
    if(HP_ <= 0){
        isAlive_ = false;
    }

    isDamaged_ = true;
}


//////////////////////////////////////////////////////////////////////////
// ジャンプ処理
//////////////////////////////////////////////////////////////////////////
void BaseCharacter::Jump(){
    if(isJump_){
        AddWorldTranslate(Vector3(0.0f, 1.0f, 0.0f) * jumpPower_);
    }
}
