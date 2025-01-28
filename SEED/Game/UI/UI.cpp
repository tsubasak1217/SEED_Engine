#include "UI.h"

/// local
// state
#include "State/IUiState.h"
// externals
#include "../adapter/json/JsonCoordinator.h"
#include "imgui.h"

UI::UI(const std::string& _name)
    : name_(_name){}

UI::~UI(){}

void UI::Initialize(const std::string& _filePath){
    sprite_ = std::make_unique<Sprite>(_filePath);

    // Jsonから読み込み
    //JsonCoordinator::LoadGroup(name_);
    // Jsonに 登録
    //JsonCoordinator::RegisterItem(name_,"Size",sprite_->size);
    //JsonCoordinator::RegisterItem(name_,"Scale",sprite_->scale);
    //JsonCoordinator::RegisterItem(name_,"Rotate",sprite_->rotate);
    //JsonCoordinator::RegisterItem(name_,"Translate",sprite_->translate);
    //JsonCoordinator::RegisterItem(name_,"AnchorPoint",sprite_->anchorPoint);
    //JsonCoordinator::RegisterItem(name_,"Color",sprite_->color);
}

void UI::Update(){
    if(state_){
        state_->Update();
    }
}

void UI::Draw(){
    sprite_->Draw();
    if(state_){
        state_->Draw();
    }
}

void UI::Finalize(){

}

void UI::BeginFrame(){
#ifdef _DEBUG
    ImGui::Begin(name_.c_str());
    JsonCoordinator::RenderGroupUI(name_);
    if(ImGui::Button("Save")){
        JsonCoordinator::SaveGroup(name_);
    }
    ImGui::End();
#endif // _DEBUG
}

void UI::EndFrame(){
    if(state_){
        state_->EndFrame();
    }
}
