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

struct TextItem{
    Vector2 offset;
    std::string typeName;
    TextBox2D textBox;
};

struct ResultItem{
    TransformSegment2D transformSegment;
    Transform2D finalTransform;
    Sprite backSprite;
    std::vector<TextItem> textItems;
    bool isAlphaMove = false;
    float maxAlpha = 1.0f;
    int32_t step = 0;
};

namespace ResultTextUtils{
    static inline std::vector<std::string> textTypes = {
        "None","Score","Combo","Perfect","PerfectCount","Great","GreatCount","Good","GoodCount","Miss","MissCount",
        "SongName","Difficulty","Fast","FastCount","Late","LateCount"
    };
}


// リザルト描画クラス
struct ResultDrawer{
    static void Initialize();
    static void DrawResult(ResultStep step, float progress);
    static inline void SetResult(const PlayResult& newResult){
        result = newResult;
    }

#ifdef _DEBUG
    static void Edit();
#endif // _DEBUG

private:
    static inline PlayResult result{};
    static inline bool initialized = false;
    static inline std::unordered_map<std::string, ResultItem> resultItems{};
    static inline bool isEditTransform = false;
    static inline float editT = 0.0f;

private:
    static void SaveToJson();
    static void LoadFromJson();
    static std::string DecideText(const std::string& typeName);
};