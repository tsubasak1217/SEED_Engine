#pragma once
#include <SEED/Lib/Structs/TextBox.h>
#include <cstdint>
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <SEED/Lib/Structs/Timer.h>

/// <summary>
/// コンボのテキストを管理する構造体
/// </summary>
struct ComboObject{
    int32_t comboCount = 0;
    GameObject2D* comboTextObj = nullptr;
    TextBox2D* text = nullptr;
    Timer scalingTimer;

public:
    ComboObject();
    void Update();

private:
    void Initialize();
};