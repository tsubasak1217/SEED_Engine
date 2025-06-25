#pragma once
#include <SEED/Source/Manager/DxManager/PSO/IPipeline.h>
#include <SEED/Source/Manager/DxManager/PSO/RootSignature.h>
#include <SEED/Lib/Structs/blendMode.h>
#include <memory>

struct PSO{
    std::unique_ptr<IPipeline> pipeline;
    std::unique_ptr<RootSignature> rootSignature;
    BlendMode GetBlendMode() const {
        return pipeline->blendMode_;
    }
};
