#pragma once

//============================================================================
//	include
//============================================================================
#include "../IPostProcess.h"

namespace SEED{
    //============================================================================
    //	Glitch class
    //============================================================================
    class Glitch :
        public IPostProcess{
    public:
        //========================================================================
        //	public Methods
        //========================================================================

        Glitch();
        ~Glitch() = default;

        void Initialize() override;

        void Update() override;

        void Apply() override;
        void Release() override;

        void StartTransition() override{};
        void EndTransition() override{};

        void Edit() override;

        nlohmann::json ToJson() override;
        void FromJson(const nlohmann::json& json) override;

        //--------- variables ----------------------------------------------------

        float time_ = 0.0f;        // 経過秒
        float intensity_ = 0.0f;   // グリッチ全体の強さ
        float blockSize_ = 32.0f;  // 横ずれブロックの太さ
        float colorOffset_ = 8.0f; // RGBずれ距離

        float noiseStrength_ = 0.01f; // ノイズ混合率
        float lineSpeed_ = 4.0f;      // スキャンライン走査速度
    };
}