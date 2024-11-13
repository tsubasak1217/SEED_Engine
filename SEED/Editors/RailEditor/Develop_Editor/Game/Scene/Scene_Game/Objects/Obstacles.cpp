#include "Obstacles.h"
#include "Baloon.h"
#include "UFO.h"
#include "Ghost.h"
#include "InputManager.h"
#include <SEED.h>
#include "MyFunc.h"

Obstacles::Obstacles(){
    Initialize();
}

Obstacles::~Obstacles(){}

void Obstacles::Initialize(){
    obstacles_.clear();
    LoadFromJson();
}

void Obstacles::Update(){

#ifdef _DEBUG
    if(InputManager::IsTriggerKey(DIK_R)){
        Initialize();
    }

    EditByImGui();
#endif // _DEBUG


    for(auto& obstacle : obstacles_){
        obstacle->Update();
    }

    for(auto& particle : particles_){
        particle->Update();
    }

    CheckCollision();

    // 条件を満たしたオブジェクトの削除
    obstacles_.remove_if([](auto& obstacle){return !obstacle->GetIsAlive(); });
    particles_.remove_if([](auto& particle){return !particle->GetIsAlive(); });
}

void Obstacles::Draw(){
    for(auto& obstacle : obstacles_){
        if(pPlayer_->pRailCamera_->GetIsRailCameraActive()){
            if(obstacle->GetIsActive()){
                obstacle->Draw();
            }
        } else{
            obstacle->Draw();
        }
    }

    for(auto& particle : particles_){
        particle->Draw();
    }
}

void Obstacles::EditByImGui(){
    if(ImGui::CollapsingHeader("Obstacles")){

        if(ImGui::Button("Output to json")){
            OutputToJson();
        }
        if(ImGui::Button("Add Baloon")){
            obstacles_.push_back(std::make_unique<Baloon>());
            Vector3 emitPos = SEED::GetCamera()->transform_.translate_ + SEED::GetCamera()->normal_ * 5.0f;
            obstacles_.back()->model_->translate_ = emitPos;
        }
        if(ImGui::Button("Add Ghost")){
            obstacles_.push_back(std::make_unique<Ghost>());
            Vector3 emitPos = SEED::GetCamera()->transform_.translate_ + SEED::GetCamera()->normal_ * 5.0f;
            obstacles_.back()->model_->translate_ = emitPos;
        }
        if(ImGui::Button("Add UFO")){
            obstacles_.push_back(std::make_unique<UFO>());
            Vector3 emitPos = SEED::GetCamera()->transform_.translate_ + SEED::GetCamera()->normal_ * 5.0f;
            obstacles_.back()->model_->translate_ = emitPos;
        }

        for(auto& obstacle : obstacles_){
            if(ImGui::CollapsingHeader(std::to_string(obstacle->obstacleID_).c_str())){
                if(ImGui::Button("Delete")){
                    obstacles_.remove(obstacle);
                    break;
                }

                ImGui::Text("Position");
                ImGui::DragFloat3("x", &obstacle->model_->translate_.x, 0.1f);

                float sin = std::sinf(3.14f * ClockManager::TotalTime() * 3.0f);
                obstacle->model_->scale_ = Vector3(1.0f, 1.0f, 1.0f) * (sin * 0.2f + 1.0f);
            }
        }
    } else{
        //for(auto& obstacle : obstacles_){
        //    obstacle->model_->scale_ = Vector3(1.0f, 1.0f, 1.0f);
        //}
    }
}

void Obstacles::OutputToJson(){
    nlohmann::json j;
    for(auto& obstacle : obstacles_){
        nlohmann::json item;

        item["type"] = static_cast<int>(obstacle->type_);
        item["position"]["x"] = obstacle->model_->translate_.x;
        item["position"]["y"] = obstacle->model_->translate_.y;
        item["position"]["z"] = obstacle->model_->translate_.z;

        j["obstacles"].push_back(item);
    }

    std::string directory = "resources/jsons/obstacles/";
    std::filesystem::create_directories(directory); // ディレクトリが存在しない場合は作成

    std::ofstream file(directory + "obstacles.json");
    file << j.dump(4);
    file.close();
}

void Obstacles::LoadFromJson(){
    std::ifstream file("resources/jsons/obstacles/obstacles.json");
    if(!file.is_open()){
        return;
    }

    nlohmann::json j;
    file >> j;

    for(const auto& item : j["obstacles"]){
        EnemyType type = static_cast<EnemyType>(item["type"]);

        switch(type){
        case EnemyType::Baloon:
            obstacles_.push_back(std::make_unique<Baloon>());
            break;
        case EnemyType::Ghost:
            obstacles_.push_back(std::make_unique<Ghost>());
            break;
        case EnemyType::UFO:
            obstacles_.push_back(std::make_unique<UFO>());
            break;
        default:
            break;
        }

        obstacles_.back()->model_->translate_.x = item["position"]["x"];
        obstacles_.back()->model_->translate_.y = item["position"]["y"];
        obstacles_.back()->model_->translate_.z = item["position"]["z"];
    }
}

void Obstacles::CheckCollision(){

    if(!pPlayer_->pRailCamera_->GetIsRailCameraActive()){ return; }

    for(auto& obstacle : obstacles_){

        float distance = MyMath::Length(obstacle->model_->translate_ - pPlayer_->player_->translate_);

        // 近づいたらアクティブに
        if(distance < effectDistance_){
            obstacle->SetIsActive(true);
        }

        if(obstacle->GetIsActive()){
            if(pPlayer_->isBeam_){
                bool isHit = MyMath::CollisionLine_Point(
                    pPlayer_->player_->translate_, pPlayer_->player_->translate_ + pPlayer_->beamVec_, obstacle->model_->translate_, obstacle->radius_
                );

                if(isHit){
                    obstacle->SetIsAlive(false);
                    pPlayer_->AddPoint(obstacle->GetPoint());

                    Vector3 pos = obstacle->model_->translate_;

                    for(int i = 0; i < 12; i++){

                        Vector3 direction = MyMath::Normalize({ MyFunc::Random(-100.0f,100.0f),MyFunc::Random(-100.0f,100.0f),MyFunc::Random(-100.0f,100.0f) });
                        particles_.push_back(std::make_unique<Bullet>(pos,direction));
                    }
                }
            }
        }
    }
}
