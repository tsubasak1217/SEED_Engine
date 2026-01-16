#pragma once
#include "../IPostProcess.h"

namespace SEED{
    /// <summary>
    /// 走査線を掛けるクラス
    /// </summary>
    class ScanLine : public IPostProcess{
    public:
        ScanLine();
        ~ScanLine() override = default;

    public:
        void Initialize() override;
        void Update() override;
        void Apply() override;
        void Release() override;
        void StartTransition() override{};
        void EndTransition() override{};

    private:
        float time_ = 0.0f;
        float stripeFrequency_ = 0.6f;
        float scanLineStrength_ = 0.2f;

    public:
        void Edit() override;
        nlohmann::json ToJson() override;
        void FromJson(const nlohmann::json& json) override;
    };
}