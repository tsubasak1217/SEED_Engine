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

struct ResultItem{
    TransformSegment2D transformSegment;
    Sprite backSprite;
    std::unordered_map<std::string, TextBox2D> textBoxes;
};

// リザルト描画クラス
struct ResultDrawer{
    static PlayResult result;
    static void Initialize();
    static void DrawResult(ResultStep step, float progress);

#ifdef _DEBUG
    static void Edit();
#endif // _DEBUG

private:
    static inline bool initialized = false;
    static inline std::unordered_map<std::string, ResultItem> resultItems{};
    static inline bool isEditTransform = false;

private:
    static void SaveToJson();
    static void LoadFromJson();
};