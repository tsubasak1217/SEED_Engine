#pragma once
#include "blendMode.h"

struct ID3D12RootSignature;
struct ID3D12PipelineState;

enum class PolygonTopology : int{
    TRIANGLE,
    LINE
};

class PSOManager{

private:
    // コピー・ムーブ禁止
    PSOManager(const PSOManager&) = delete;
    void operator=(const PSOManager&) = delete;
    void operator=(PSOManager&&) = delete;

    // privateコンストラクタ
    PSOManager() = default;

    // インスタンス
    static PSOManager* instance_;

public:
    static PSOManager* GetInstance();
    ~PSOManager();

    static ID3D12RootSignature* SettingCSRootSignature();

    static void Create(
        ID3D12RootSignature** pRootSignature, ID3D12PipelineState** pPipelineState,
        PolygonTopology topology = PolygonTopology::TRIANGLE,
        BlendMode blendMode = BlendMode::NORMAL
    );
};