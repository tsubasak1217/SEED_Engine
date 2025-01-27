#pragma once

// host
class UI;

///stl
//string
#include <string>

class IUiState{
public:
    IUiState(UI* _ui,const std::string& _stateName): ui_(_ui),stateName_(_stateName){}
    virtual ~IUiState(){}

    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;

    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;

protected:
    UI* ui_;
    std::string stateName_;
public:
    const std::string& GetStateName()const{ return stateName_; }
};