#pragma once
///stl
//pointer
#include <memory>

///local
//parent
#include "State_Base.h"
//host
class Scene_Title;
#include "Scene_Title.h"


class ITitleState
    : public State_Base{
public:
    ITitleState(Scene_Title* host);
    virtual ~ITitleState();

    void Initialize(){};
    void Update() = 0;
    void Draw() = 0;
    void Finalize(){};

    void BeginFrame() = 0;
    void EndFrame() = 0;

    void HandOverColliders() = 0;
    void ManageState() = 0;
protected:
    Scene_Title* host_;
};

