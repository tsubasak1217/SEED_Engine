#pragma once
// stl
#include <unordered_map>
#include <array>
#include <vector>
#include <regex>
// local
#include <SEED/Source/Manager/DxManager/ShaderDictionary.h>
#include <SEED/Lib/Structs/blendMode.h>
#include "Pipeline.h"
#include "MSPipeline.h"
#include "RootSignature.h"
#include "PSO.h"

struct ID3D12RootSignature;
struct ID3D12PipelineState;

// パイプラインの種類
enum class PipelineType{
    VSPipeline = 0,
    MSPipeline,
    CSPipeline,
};

//////////////////////////////////////////////////////////////////////
// パイプラインの管理クラス
//////////////////////////////////////////////////////////////////////
class PSOManager{

private:
    static const int kTopologyCount = 2;
    static const int kBlendModeCount = (int)BlendMode::kBlendModeCount;
    static const int kCullModeCount = 3;

    struct PSODictionary{
        std::array<
            std::array<
                std::array<std::unique_ptr<PSO>, kBlendModeCount>,
            kCullModeCount>,
        kTopologyCount> dict_;// パイプラインの辞書
    };

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
    void Release();

public:

    // PSOの取得
    static PSO* GetPSO(const std::string& name, BlendMode blendMode, D3D12_CULL_MODE cullMode, PolygonTopology topology);
    static PSO* GetPSO_Compute(const std::string& name);

    // ファイルからPSOを生成
    static void CreatePipelines(const std::string& filename);

private:
    // PSOの生成
    static void Create(RootSignature* pRootSignature, IPipeline* pPipeline,bool isCSPipeline = false);

    // シェーダー情報からRootSignatureの情報を作成する関数
    void GenerateRootParameters(
        RootSignature* rootSignature,
        const std::string& shaderName,ID3D12ShaderReflection* reflection
    );
    void GenerateInputLayout(Pipeline* pipeline, ID3D12ShaderReflection* reflection);
    DXGI_FORMAT GetFormatFromSignatureDesc(const D3D12_SIGNATURE_PARAMETER_DESC& desc);

private:
    std::unordered_map<std::string, PSODictionary> psoDictionary_;// パイプラインの辞書
    std::unordered_map<std::string, std::unique_ptr<PSO>> csPsoDictionary_;// CSのパイプラインの辞書
};