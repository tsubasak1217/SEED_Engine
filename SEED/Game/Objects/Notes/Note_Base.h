#pragma once
#include <vector>
#include <cstdint>
#include <memory>
#include <json.hpp>
#include <SEED/Lib/Shapes/Quad.h>
#include <SEED/Lib/enums/Direction.h>
#include <Game/Objects/Judgement/Judgement.h>
#include <Game/Objects/Judgement/PlayerInput.h>
#include <Game/Objects/Judgement/PlayField.h>
#include <Game/Objects/Judgement/LaneBit.h>

// ノーツの種類を表すenum
enum class NoteType{
    None = -1,
    Tap,
    Hold,
    SideFlick,
    RectFlick,
    Wheel,
    Warning,
};


/// <summary>
/// ノーツの基底クラス
/// </summary>
class Note_Base{
public:
    Note_Base();
    virtual ~Note_Base() = default;
    virtual void Update() = 0;
    virtual void Draw(float currentTime,float appearLength);
    virtual Judgement::Evalution Judge(float dif) = 0;

    // 入出力関数
    virtual nlohmann::json ToJson() = 0;
    virtual void FromJson(const nlohmann::json& json) = 0;

#ifdef _DEBUG
    virtual void Edit() = 0;
#endif // _DEBUG

public:
    NoteType noteType_ = NoteType::None;// ノーツの種類
    int32_t lane_;// レーン番号
    UpDown layer_ = UpDown::NONE;// 上下の方向
    LaneBit laneBit_;// レーンのビットフラグ
    float time_;// 出現時間
    std::vector<std::shared_ptr<Note_Base>> controlPoints_;// スライドノーツなどの制御点の情報
    bool isExtraNote_;// 判定が甘いノーツかどうか
    inline static int32_t nextNoteID_ = 0;// 次のノーツID
    int32_t noteID_;// ノーツのID
    bool isEnd_ = false;// ノーツが終わったかどうか
    inline static float zOffset_ = -0.005f;// ノーツのZ軸オフセット

#ifdef _DEBUG
    bool isDragging_ = false;
#endif // _DEBUG


protected:
    std::unique_ptr<Quad> noteQuad_;// ノーツの画像
};