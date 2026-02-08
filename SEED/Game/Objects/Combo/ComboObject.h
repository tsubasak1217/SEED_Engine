#pragma once
#include <SEED/Lib/Structs/TextBox.h>
#include <cstdint>
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <SEED/Lib/Structs/Timer.h>

/// <summary>
/// コンボのテキストを管理するクラス
/// </summary>
class ComboObject{
    int32_t comboCount_ = 0;
    SEED::GameObject2D* comboTextObj_ = nullptr;
    SEED::TextBox2D* text_ = nullptr;
    SEED::Timer scalingTimer_;

public:// 基本関数
    ComboObject();
    void Update();

public:// アクセッサ

    int32_t GetComboCount()const{ return comboCount_; }

    void AddCombo(){
        comboCount_++;
        scalingTimer_.Reset();
    }

    void BreakCombo(){
        comboCount_ = 0;
    }

private:// 内部関数
    void Initialize();
};