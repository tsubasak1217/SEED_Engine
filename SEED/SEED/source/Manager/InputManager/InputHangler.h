#pragma once
#include <functional>

// 入力ハンドラ
template<typename T>
struct InputHandler{
    std::function<T()> Value = [] { return T{}; }; // デフォルトはゼロ初期化
    std::function<bool()> Press = [] { return false; };
    std::function<bool()> Trigger = [] { return false; };
    std::function<bool()> Release = [] { return false; };
};