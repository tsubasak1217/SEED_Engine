#pragma once
#include <SEED/Lib/Structs/blendMode.h>
#include "Pipeline.h"
#include "RootSignature.h"

struct ID3D12RootSignature;
struct ID3D12PipelineState;

// パイプラインの種類
enum class PippelineType{
    Normal = 0,
    Skinning,
};

//////////////////////////////////////////////////////////////////////
// パイプラインの管理クラス
//////////////////////////////////////////////////////////////////////
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

public:

    // ConputeShaderのルートシグネチャ作成
    static ComPtr<ID3D12RootSignature> SettingCSRootSignature();

    // テンプレートパラメータの生成
    static void GenerateTemplateParameter(RootSignature* pRootSignature, Pipeline* pPipeline,PippelineType pypelineType);

    // PSOの生成
    static void Create(RootSignature* pRootSignature,Pipeline* pPipeline);

};