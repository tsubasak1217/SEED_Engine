#pragma once
#include "../IPostProcess.h"
#include "FogParam.h"

namespace SEED{
    /// <summary>
    /// フォグを掛けるクラス
    /// </summary>
    class Fog : public IPostProcess{
    public:
        Fog();
        ~Fog() override = default;

    public:
        void Initialize() override;
        void Update() override;
        void Apply() override;
        void Release() override;
        void StartTransition() override{};
        void EndTransition() override{};

    private:
        DxBuffer<FogParams> fogParamsBuffer_; // ブラーのパラメータを格納するバッファ

    public:
        void Edit() override;
        nlohmann::json ToJson() override;
        void FromJson(const nlohmann::json& json) override;
    };
}