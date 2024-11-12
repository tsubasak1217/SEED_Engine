#include "PlayerGage.h"
#include <json.hpp>

PlayerGage::PlayerGage(){
    Initialize();
}

PlayerGage::~PlayerGage(){}

void PlayerGage::Initialize(){
    shotGage_ = std::make_unique<Sprite>("shotGage.png");
    shotGageFrame_ = std::make_unique<Sprite>("shotGageFrame.png");
    shotGage_->anchorPoint = { 0.0f,1.0f };
    shotGageFrame_->anchorPoint = { 0.0f,1.0f };

    scoreGageBack_ = std::make_unique<Sprite>("scoreBack.png");

    LoadFromJson();
}


// ショットゲージの更新 
void PlayerGage::Update(){

    // スコアの計算
    CalcScoreGage();

    // ImGuiでの編集
    EditByImGui();

    // スコアのスプライトの配置
    for(int i = 0; i < score_.size(); i++){
        score_[i]->translate = scoreBasePos_ + scoreOffset_ * float(i);
        score_[i]->scale = scoreBaseScale_;
    }
}


void PlayerGage::Draw(){
    // ショットゲージ
    shotGage_->Draw();

    // ショットゲージのフレーム
    shotGageFrame_->Draw();

    // スコアゲージの背景
    scoreGageBack_->Draw();

    // スコアゲージ
    for(auto& score : score_){
        score->Draw();
    }

}

// スコアゲージの計算
void PlayerGage::CalcScoreGage(){

    // スコアの桁数を計算
    int countOfDigit = 1;
    int digit = 1;
    int score = pPlayer_->score_;
    while(true){
        if(score / digit == 0){
            break;
        }

        countOfDigit++;
        digit *= 10;
    }

    // スコアの計算
    digit = 1;
    for(int i = 0; i < countOfDigit; i++){

        if(score_.size() == i){
            if(i != countOfDigit - 1){
                score_.emplace_back(std::make_unique<Sprite>("score.png"));
            } else{
                break;
            }
        }
        
        int num = (score / digit) % 10;
        score_[i]->clipLT = { numberGraphWidth_ * num,0.0f };
        score_[i]->clipSize = { 42.0f,71.0f };
        digit *= 10;
    }
}

void PlayerGage::StretchShotGage(){
    shotGage_->clipLT = { 0.0f,420.0f * (1.0f - pPlayer_->shotGage_) };

}

// ImGuiでの編集
void PlayerGage::EditByImGui(){
    ImGui::Begin("PlayerGage");

    ImGui::DragFloat2("shotGagePos", &shotGage_->translate.x, 1.0f);
    ImGui::DragFloat2("shotGageScale", &shotGage_->scale.x, 0.1f);
    shotGageFrame_->translate = shotGage_->translate;
    shotGageFrame_->scale = shotGage_->scale;

    ImGui::DragFloat2("scoreGageBackPos", &scoreGageBack_->translate.x, 1.0f);
    ImGui::DragFloat2("scoreGageBackScale", &scoreGageBack_->scale.x, 0.1f);

    ImGui::DragFloat2("scoreBasePos", &scoreBasePos_.x, 1.0f);
    ImGui::DragFloat2("scoreBaseScale", &scoreBaseScale_.x, 0.1f);

    ImGui::DragFloat2("scoreOffset", &scoreOffset_.x, 0.1f);

    if(ImGui::Button("Output to json")){
        OutputToJson();
    }

    ImGui::End();

}

void PlayerGage::OutputToJson(){
    nlohmann::json j;

    j["shotGage"]["translate"]["x"] = shotGage_->translate.x;
    j["shotGage"]["translate"]["y"] = shotGage_->translate.y;
    j["shotGage"]["scale"]["x"] = shotGage_->scale.x;
    j["shotGage"]["scale"]["y"] = shotGage_->scale.y;

    j["scoreGageBack"]["translate"]["x"] = scoreGageBack_->translate.x;
    j["scoreGageBack"]["translate"]["y"] = scoreGageBack_->translate.y;
    j["scoreGageBack"]["scale"]["x"] = scoreGageBack_->scale.x;
    j["scoreGageBack"]["scale"]["y"] = scoreGageBack_->scale.y;

    j["scoreBasePos"]["x"] = scoreBasePos_.x;
    j["scoreBasePos"]["y"] = scoreBasePos_.y;
    j["scoreBaseScale"]["x"] = scoreBaseScale_.x;
    j["scoreBaseScale"]["y"] = scoreBaseScale_.y;
    j["scoreOffset"]["x"] = scoreOffset_.x;
    j["scoreOffset"]["y"] = scoreOffset_.y;

    ///////////////////////////////////////////////
    // ファイル出力
    ///////////////////////////////////////////////
    std::string directory = "resources/jsons/UI_Information/";
    std::filesystem::create_directories(directory); // ディレクトリが存在しない場合は作成

    std::ofstream outputFile(directory + "PlayerGage.json");
    if(!outputFile.is_open()) {
        throw std::runtime_error("Failed to open PlayerGage.json");
    }

    outputFile << j.dump(4);
}


// jsonファイルからパラメーターを読み込む
void PlayerGage::LoadFromJson(){
    std::ifstream inputfile("resources/jsons/UI_Information/PlayerGage.json");
    if(!inputfile.is_open()){
        return;
    }

    nlohmann::json j;
    inputfile >> j;

    shotGage_->translate.x = j["shotGage"]["translate"]["x"];
    shotGage_->translate.y = j["shotGage"]["translate"]["y"];
    shotGage_->scale.x = j["shotGage"]["scale"]["x"];
    shotGage_->scale.y = j["shotGage"]["scale"]["y"];

    shotGageFrame_->translate = shotGage_->translate;
    shotGageFrame_->scale = shotGage_->scale;

    scoreGageBack_->translate.x = j["scoreGageBack"]["translate"]["x"];
    scoreGageBack_->translate.y = j["scoreGageBack"]["translate"]["y"];
    scoreGageBack_->scale.x = j["scoreGageBack"]["scale"]["x"];
    scoreGageBack_->scale.y = j["scoreGageBack"]["scale"]["y"];

    scoreBasePos_.x = j["scoreBasePos"]["x"];
    scoreBasePos_.y = j["scoreBasePos"]["y"];
    scoreBaseScale_.x = j["scoreBaseScale"]["x"];
    scoreBaseScale_.y = j["scoreBaseScale"]["y"];
    scoreOffset_.x = j["scoreOffset"]["x"];
    scoreOffset_.y = j["scoreOffset"]["y"];
}

