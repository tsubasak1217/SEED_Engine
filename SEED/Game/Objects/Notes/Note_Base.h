#pragma once
#include <vector>
#include <cstdint>
#include <memory>
#include <SEED/Lib/Structs/Sprite.h>

class Note_Base{
public:
    Note_Base() = default;
    virtual ~Note_Base() = default;
    virtual void Update() = 0;
    virtual void Draw();

public:
    uint32_t lane_;// レーン番号
    float time_;// 出現時間
    std::vector<std::shared_ptr<Note_Base>> controlPoints_;// スライドノーツなどの制御点の情報
    bool isExtraNote_;// 判定が甘いノーツかどうか

protected:
    std::unique_ptr<Sprite> noteSprite_;// ノーツの画像
};