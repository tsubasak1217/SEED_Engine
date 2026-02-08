#pragma once
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Lib/Structs/Curve.h>
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Lib/enums/ColorMode.h>

namespace SEED{
    /// <summary>
    /// 色を制御するコンポーネント
    /// </summary>
    class ColorControlComponent : public IComponent{
    public:// 基礎関数
        ColorControlComponent(std::variant<GameObject*, GameObject2D*> pOwner, const std::string& tagName = "");
        ~ColorControlComponent() = default;
        void Initialize()override;
        void BeginFrame() override;
        void Update() override;
        void Draw() override;
        void EndFrame() override;
        void Finalize() override;

    public:// アクセッサ
        void SetTimeScale(float timeScale){ timeScale_ = timeScale; }
        void RevercePlay(){ timeScale_ *= -1.0f; }

    public:// 入出力
        // GUI編集
        void EditGUI() override;
        // json
        nlohmann::json GetJsonData() const override;
        void LoadFromJson(const nlohmann::json& jsonData) override;

    private:
        Curve colorCurve_;
        GeneralEnum::ColorMode colorControlMode_ = GeneralEnum::ColorMode::RGBA;
        Timer timer_;
        float timeScale_ = 1.0f;
        bool isLoop_ = false;
        bool isMultiply_ = false;// 直接書き換えるか乗算するかを管理
    };
}