#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <Game/Objects/Result/PlayResult.h>
#include <SEED/Lib/Structs/TextBox.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <Game/GameSystem.h>

//enum class ResultStep : int32_t{
//    Appear = 0,
//    CountUp,
//    Display,
//    Exit
//};

//namespace ResultTextUtils{
//    static inline std::vector<std::string> textTypes = {
//        "None","Score","Combo","Perfect","PerfectCount","Great","GreatCount","Good","GoodCount","Miss","MissCount",
//        "SongName","Difficulty","Fast","FastCount","Late","LateCount"
//    };
//}


// リザルト描画クラス
class ResultDrawer{
public:
    void Update();
    void Draw();
    void Initialize();

public:
    static void SetResult(const PlayResult& result){ playResult_ = result; }

private:
    static inline PlayResult playResult_{};
    LoadObjectData loadObjects_;
};