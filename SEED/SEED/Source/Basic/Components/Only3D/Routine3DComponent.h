#pragma once
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/enums/InterpolateType.h>
// stl
#include <memory>
#include <vector>

using RoutinePoint3D = std::pair<SEED::Transform, float>;// 座標とその座標までの時間

namespace SEED{
    /// <summary>
    /// 3次元のルーチンポイントを設定するコンポーネント
    /// </summary>
    class Routine3DComponent : public IComponent{
    public:
        Routine3DComponent(GameObject* pOwner, const std::string& tagName = "");
        ~Routine3DComponent() = default;
        void BeginFrame()override;
        void Update()override;
        void Draw()override;
        void EndFrame()override;
        void Finalize()override;
        void EditGUI()override;

    public:// accessor
        void Play(){ isPlaying_ = true; }
        void Pause(){ isPlaying_ = false; }
        void Reset(){ timer_.Reset(); }
        void RevercePlay(){ timeScale_ *= -1.0f; }
        bool IsEndRoutine() const{ return timer_.IsFinished(); }
        const Transform& GetControlPoint(size_t index) const;
        const std::vector<RoutinePoint3D>& GetControlPoints() const{ return controlPoints_; }
        const Timer& GetTimer()const{ return timer_; }

    public:// json関連
        void LoadFromJson(const nlohmann::json& jsonData) override;
        nlohmann::json GetJsonData() const override;

    private:
        bool defaultPaused_ = false;
        bool isLoop_ = false;
        bool isConnectEdge_ = false;
        bool isPlaying_ = false;
        bool disableTranslate_ = false;
        bool disableRotate_ = false;
        bool disableScale_ = false;
        Timer timer_;
        float timeScale_ = 1.0f;
        GeneralEnum::InterpolationType interpolationType_ = GeneralEnum::InterpolationType::CATMULLROM;
        std::vector<RoutinePoint3D> controlPoints_;
        Methods::Easing::Type easingType_ = Methods::Easing::Type::None;

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
        Model debugPointModel_;
        Vector3 eulerRotate_;

    #endif // _DEBUG
    };
}