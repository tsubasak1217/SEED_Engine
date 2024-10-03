#pragma once

class DxManager;
struct ID3D12RootSignature;
struct ID3D12PipelineState;

enum class BlendMode : int{
    NONE,
    NORMAL,
    ADD,
    SUBTRACT,
    MULTIPLY,
    SCREEN
};

enum class PolygonTopology : int{
    TRIANGLE,
    LINE
};

struct PSOManager{

    PSOManager(DxManager* pDxManager);
    ~PSOManager();
    
    ID3D12RootSignature* SettingCSRootSignature();
    void Create(
        ID3D12RootSignature** pRootSignature,ID3D12PipelineState** pPipelineState,
        PolygonTopology topology = PolygonTopology::TRIANGLE,
        BlendMode blendMode = BlendMode::NORMAL
    );

private:
    DxManager* pDxManager_;
};