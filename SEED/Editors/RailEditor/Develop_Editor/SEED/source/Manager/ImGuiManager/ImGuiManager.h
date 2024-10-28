#pragma once
#include "DxManager.h"

class SEED;

class ImGuiManager{
public:
    void Initialize();
    void Finalize();

    void Draw();

    void Begin();
    void End();

};