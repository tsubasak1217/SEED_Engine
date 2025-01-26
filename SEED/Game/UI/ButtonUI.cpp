#include "ButtonUI.h"

/// local
// lib
#include "InputManager.h"

ButtonUI::ButtonUI(const std::string& _name): UI(_name){}

ButtonUI::~ButtonUI(){}

void ButtonUI::Initialize(const std::string& _filePath){
    UI::Initialize(_filePath);

    input_ = Input::GetInstance();
}

void ButtonUI::Update(){}

void ButtonUI::Draw(){}

void ButtonUI::Finalize(){}

void ButtonUI::BeginFrame(){}

void ButtonUI::EndFrame(){}
