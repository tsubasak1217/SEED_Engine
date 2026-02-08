#pragma once
#include "../IPostProcess.h"

namespace SEED{
    /// <summary>
    /// ビネットを掛けるクラス
    /// </summary>
    class Vignette : public IPostProcess{
    public:
        Vignette();
        ~Vignette() override = default;

    public:
        void Initialize() override;
        void Update() override{};
        void Apply() override;
        void Release() override;
        void StartTransition() override{};
        void EndTransition() override{};

    private:
        float vignetteStrength_ = 1.0f;

    public:
        void Edit() override;
        nlohmann::json ToJson() override;
        void FromJson(const nlohmann::json& json) override;
    };
}