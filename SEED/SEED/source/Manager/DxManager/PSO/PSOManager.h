#pragma once
// stl
#include <unordered_map>
#include <array>
#include <vector>
#include <regex>
#include <variant>
// local
#include <SEED/Source/Manager/DxManager/ShaderDictionary.h>
#include <SEED/Lib/Structs/blendMode.h>
#include "Pipeline.h"
#include "MSPipeline.h"
#include "RootSignature.h"
#include "PSO.h"

// 前方宣言
struct ID3D12RootSignature;
struct ID3D12PipelineState;

namespace SEED{

    // パイプラインの種類
    enum class PipelineType{
        VSPipeline = 0,
        MSPipeline,
        CSPipeline,
    };

    //////////////////////////////////////////////////////////////////////
    // PSOを管理するクラス
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

        // RootSignatureに対してバインド情報を設定する関数
        static void SetBindInfo(
            const std::string& pipelineName, const std::string& variableName,
            std::variant<D3D12_GPU_DESCRIPTOR_HANDLE, D3D12_GPU_VIRTUAL_ADDRESS, void*> info
        );

    private:
        // PSOの生成
        static void Create(RootSignature* pRootSignature, IPipeline* pPipeline, bool isCSPipeline = false);

        // シェーダー情報からRootSignatureの情報を作成する関数
        void GenerateRootParameters(
            RootSignature* rootSignature,
            const std::string& shaderName, ID3D12ShaderReflection* reflection
        );
        void GenerateInputLayout(Pipeline* pipeline, ID3D12ShaderReflection* reflection);
        DXGI_FORMAT GetFormatFromSignatureDesc(const D3D12_SIGNATURE_PARAMETER_DESC& desc);

    private:
        std::unordered_map<std::string, PSODictionary> psoDictionary_;// パイプラインの辞書
        std::unordered_map<std::string, std::unique_ptr<PSO>> csPsoDictionary_;// CSのパイプラインの辞書
    };
}