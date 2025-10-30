#pragma once
#include <Game/GameSystem.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <Game/Objects/Notes/NotesData.h>

/// <summary>
/// マウスの移動量を表示する円のクラス
/// </summary>
class MouseVectorCircle{
public:
    MouseVectorCircle() = default;
    ~MouseVectorCircle() = default;

public:
    void Initialize();
    void Update();

public:
    void SetNotesData(const NotesData* pNotesData);

private:
    GameObject2D* circleObj_ = nullptr;
    GameObject2D* cursorObj_ = nullptr;
    const NotesData* pNotesData_ = nullptr;
    bool isScalingUp_;
    Timer scalingTimer_;
    float searchTimeWidth_ = 1.5f;
    float radius = 92.0f;
    Vector2 prevMouseVec_;
    Color circleColor_;
};