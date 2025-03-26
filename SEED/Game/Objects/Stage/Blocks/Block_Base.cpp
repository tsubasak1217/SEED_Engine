#include "Block_Base.h"
#include <Game/Objects/Stage/StageManager.h>

uint32_t Block_Base::nextBlockID_ = 0;
float Block_Base::baseBlockSize_ = 2.0f;

void Block_Base::Initialize(){

}

void Block_Base::BeginFrame(){
    preAddress_ = address_;
}

void Block_Base::Update(){
    if(blockModel_){
        blockModel_->Update();
        UpdateMatrix();
    }
}

void Block_Base::Draw(){
    if(blockModel_){
        blockModel_->Draw();
    }
}

void Block_Base::UpdateMatrix(){
    if(blockModel_){
        float blockSize = StageManager::GetInstance()->GetBlockSize();
        blockModel_->translate_ = { (float)address_.x * blockSize,0.0f,(float)-address_.y * blockSize };
        blockModel_->UpdateMatrix();
    }
}

void Block_Base::AdjustSize(){
    float scale = StageManager::GetInstance()->GetBlockSize() / baseBlockSize_;
    if(blockModel_){
        blockModel_->scale_ = { scale,scale,scale };
        blockModel_->UpdateMatrix();
    }
}

void Block_Base::Load(const nlohmann::json& json){

    // アドレスを読み込み、座標を計算
    if(json.contains("address")){
        address_ = json["address"];
        firstAddress_ = address_;
        Vector2 pos = Vector2((float)address_.x, (float)address_.y) * StageManager::GetInstance()->GetBlockSize();
        blockModel_->translate_ = { pos.x,0.0f,pos.y };
        firstPos_ = blockModel_->translate_;
    }
}

void Block_Base::Output(nlohmann::json& json){
    json;
}
