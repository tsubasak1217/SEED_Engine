#pragma once

/// stl
//pointer
#include <memory>

//local
#include "../Base/BaseObject.h"

//stage
class Stage;

class Shadow{
public:
    Shadow(BaseObject* _host);
    ~Shadow();
    void Initialize();
    void Update(Stage* currentStage);
    void Draw();

private:
    BaseObject* host_ = nullptr;
    std::unique_ptr<Model> model_ = nullptr;
};