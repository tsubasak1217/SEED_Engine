#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <Game/Objects/Result/PlayResult.h>
#include <SEED/Lib/Structs/TextBox.h>

enum class ResultStep : int32_t{
    Appear = 0,
    CountUp,
    Display,
    Exit
};

// リザルト描画クラス
struct ResultDrawer{
    static PlayResult result;
    static void DrawResult(ResultStep step, float progress);

private:
    static inline bool initialized = false;
    static inline std::unordered_map<std::string, TransformSegment2D> transformSegments{};

private:
    static void Initialize();

#ifdef _DEBUG
    void Edit();
#endif // _DEBUG

};