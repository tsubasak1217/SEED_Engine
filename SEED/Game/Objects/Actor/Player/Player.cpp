#include "Player.h"
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <Game/Objects/Stage/StageManager.h>

// static変数
int Player::playerCount_ = 0;
int32_t Player::doppelSteps_ = 10;// ドッペルゲンガーを生むまでの歩数

///////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
///////////////////////////////////////////////////////////
Player::Player(){
    Initialize();
    characterNo_ = playerCount_;
    playerCount_++;
}

Player::~Player(){
    playerCount_--;
}

///////////////////////////////////////////////////////////
// 初期化
///////////////////////////////////////////////////////////
void Player::Initialize(){
    modelColor_ = MyMath::HSV_to_RGB((float)playerCount_ / 10.0f,0.6f,1.0f,1.0f);
    blockType_ = BlockType::Player;
    blockModel_ = std::make_unique<Model>("Assets/suzanne.obj");
    blockModel_->color_ = modelColor_;
    blockModel_->isRotateWithQuaternion_ = false;

    // 移動履歴モデルの初期化
    historyModel_ = std::make_unique<Model>("Assets/sphere.obj");
    historyModel_->color_ = modelColor_;
    historyModel_->scale_ = blockModel_->scale_ * 0.7f;


    AdjustSize();
}

///////////////////////////////////////////////////////////
// フレームの開始時処理
///////////////////////////////////////////////////////////
void Player::BeginFrame(){
    Block_Base::BeginFrame();
    isMoveInput_ = false;
}

///////////////////////////////////////////////////////////
// 更新
///////////////////////////////////////////////////////////
void Player::Update(){
    if(Move() == true){
        return;
    }

    Block_Base::Update();
}

///////////////////////////////////////////////////////////
// 描画
///////////////////////////////////////////////////////////
void Player::Draw(){
    Block_Base::Draw();
    if(!isPlaying_){
        DrawHistory();
    }
}

///////////////////////////////////////////////////////////
// 読み込み
///////////////////////////////////////////////////////////
void Player::Load(const nlohmann::json& json){
    Block_Base::Load(json);
    isDoppel_ = false;// 読み込まれた奴はドッペルゲンガーではない
}

///////////////////////////////////////////////////////////
// 衝突解決
///////////////////////////////////////////////////////////
void Player::SolveCollision(Block_Base* other){

    // 衝突したブロックの種類を取得
    BlockType otherType = other->GetBlockType();

    // プレイヤーの場合
    if(otherType == BlockType::Player){
        Player* otherPlayer = static_cast<Player*>(other);

        // こちらからの入力で衝突した場合、相手を殺す
        if(isMoveInput_){
            otherPlayer->isAlive_ = false;
        }
    
    } else if(otherType == BlockType::Toge){
        // トゲに衝突した場合、こちらを殺す
        isAlive_ = false;
    
    } else if(otherType == BlockType::Box){
        // ブロックに埋まった場合、こちらを殺す
        isAlive_ = false;
    }
}

///////////////////////////////////////////////////////////
// 移動
///////////////////////////////////////////////////////////
bool Player::Move(){

    // 前後左右のみの移動
    Vector2i moveVec = { 0,0 };
    if(Input::IsTriggerKey(DIK_W)){
        moveVec.y = -1;
    } else if(Input::IsTriggerKey(DIK_S)){
        moveVec.y = 1;
    } else if(Input::IsTriggerKey(DIK_A)){
        moveVec.x = -1;
    } else if(Input::IsTriggerKey(DIK_D)){
        moveVec.x = 1;
    }


    // 移動可能なら移動
    if(moveVec != Vector2i(0, 0)){


        if(isPlaying_){

            isMoveInput_ = true;

            // 移動方向に応じて回転する
            Vector3 rotate;
            rotate = MyFunc::CalcRotateVec({ (float)moveVec.x,0.0f,(float)-moveVec.y });
            blockModel_->rotate_ = rotate;
            
            // 移動
            if(StageManager::GetInstance()->GetCurrentStage()->RequestPlayerMove(address_ + moveVec, this)){
                // 歩数を増やし、移動履歴に追加
                moveHistory_.push_back(moveVec);
                steps_++;

                if(steps_ >= doppelSteps_){
                    isCreatedDoppel_ = true;
                    isPlaying_ = false;
                    steps_ = 0;
                    StageManager::GetInstance()->GetCurrentStage()->CreateDoppelganger(address_);
                }

                return true;
            };
        } else{

            // 移動履歴から自動で移動
            if(moveHistory_.size() > 0){


                // 移動に応じて回転
                Vector2i autoMove = moveHistory_[steps_];
                Vector3 rotate;
                rotate = MyFunc::CalcRotateVec({ (float)autoMove.x,0.0f,(float)-autoMove.y });
                blockModel_->rotate_ = rotate;

                steps_++;
                if(steps_ >= doppelSteps_){
                    steps_ = 0;
                }

                // 移動
                return StageManager::GetInstance()->GetCurrentStage()->RequestPlayerMove(address_ + autoMove, this);
            }
        }
    }

    return false;

}

///////////////////////////////////////////////////////////
// 移動履歴の描画
///////////////////////////////////////////////////////////
void Player::DrawHistory(){
    Vector3 pos = firstPos_;
    float blockSize = StageManager::GetInstance()->GetBlockSize();

    // 初期地点だけ直接描画(少し大きく)
    historyModel_->translate_ = pos;
    historyModel_->scale_ = blockModel_->scale_;
    historyModel_->UpdateMatrix();
    historyModel_->Draw();

    // 大きさを元に戻す
    historyModel_->scale_ = blockModel_->scale_ * 0.7f;

    // 移動履歴の描画
    for(int i = 0; i < moveHistory_.size(); i++){
        pos += Vector3((float)moveHistory_[i].x * blockSize, 0.0f, (float)-moveHistory_[i].y * blockSize);
        historyModel_->translate_ = pos;
        historyModel_->UpdateMatrix();
        historyModel_->Draw();
    }
}

///////////////////////////////////////////////////////////
// 前方ベクトルを取得
///////////////////////////////////////////////////////////
Vector2i Player::GetForwardVec(){
    Vector3 rotatedVec = Vector3(0.0f, 0.0f, 1.0f) * RotateMatrix(blockModel_->rotate_);
    return Vector2i((int)rotatedVec.x, (int)-rotatedVec.z);
}
