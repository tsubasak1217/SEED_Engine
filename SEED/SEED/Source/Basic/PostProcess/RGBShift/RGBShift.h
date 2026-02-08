#pragma once
#include "../IPostProcess.h"

namespace SEED{
    /// <summary>
    /// RGBずらし(疑似色収差)を行うクラス
    /// </summary>
    class RGBShift : public IPostProcess{
    public:
        RGBShift();
        ~RGBShift() override = default;

    public:
        void Initialize() override;
        void Update() override{};
        void Apply() override;
        void Release() override;
        void StartTransition() override{};
        void EndTransition() override{};

    private:
        int gap_ = 2;

    public:
        void Edit() override;
        nlohmann::json ToJson() override;
        void FromJson(const nlohmann::json& json) override;
    };
}