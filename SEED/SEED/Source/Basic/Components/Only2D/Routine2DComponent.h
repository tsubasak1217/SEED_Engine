#pragma once
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/enums/InterpolateType.h>
// stl
#include <memory>
#include <vector>

using RoutinePoint2D = std::pair<Transform2D, float>;// 座標とその座標までの時間

/// <summary>
/// 2次元のルーチン設定コンポーネント
/// </summary>
class Routine2DComponent : public IComponent{
public:
    Routine2DComponent(GameObject2D* pOwner, const std::string& tagName = "");
    ~Routine2DComponent() = default;
    void BeginFrame()override;
    void Update()override;
    void Draw()override;
    void EndFrame()override;
    void Finalize()override;
    void EditGUI()override;

public:// アクセッサ
    void Play(){ isPlaying_ = true; }
    void Pause(){ isPlaying_ = false; }

public:// json関連
    void LoadFromJson(const nlohmann::json& jsonData) override;
    nlohmann::json GetJsonData() const override;

private:
    bool defaultPaused_ = false;
    bool isLoop_ = false;
    bool isPlaying_ = false;
    Timer timer_;
    InterpolationType interpolationType_ = InterpolationType::CATMULLROM;
    std::vector<RoutinePoint2D> controlPoints_;
    Easing::Type easingType_ = Easing::Type::None;

#ifdef _DEBUG
    // 編集用内部関数
    void TimelineView();
    void DragPoint();
    void EditTransform();
    void EditSettings();
    void PopupMenu();
    void AddPoint(float time);

    // 編集用変数
    int32_t edittingIdx_ = -1;
    bool isDragging_ = false;
    ImVec2 clickedMousePos_;
    ImVec2 originalPointPos_;
    ImVec2 timelineMinMaxX_;
    bool isDebugItemVisible_ = true;
    bool isEditting_ = false;
    Sprite debugPointSprite_;

#endif // _DEBUG
};