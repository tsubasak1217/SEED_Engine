#pragma once
#include "blendMode.h"

struct ID3D12RootSignature;
struct ID3D12PipelineState;

enum class PolygonTopology : int{
    TRIANGLE,
    LINE
};

class PSOManager{

public:
    PSOManager();
    ~PSOManager();

    ID3D12RootSignature* SettingCSRootSignature();
    void Create(
        ID3D12RootSignature** pRootSignature, ID3D12PipelineState** pPipelineState,
        PolygonTopology topology = PolygonTopology::TRIANGLE,
        BlendMode blendMode = BlendMode::NORMAL
    );
};