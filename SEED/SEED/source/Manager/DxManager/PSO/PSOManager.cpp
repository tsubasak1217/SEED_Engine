// local
#include <SEED/Source/Manager/DxManager/PSO/PSOManager.h>
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <SEED/Lib/Structs/blendMode.h>

/////////////////////////////////////////////////////////////////////////////////////////////
//                         static変数初期化
/////////////////////////////////////////////////////////////////////////////////////////////

PSOManager* PSOManager::instance_ = nullptr;


/////////////////////////////////////////////////////////////////////////////////////////////
//                          初期化・終了関数
/////////////////////////////////////////////////////////////////////////////////////////////

PSOManager* PSOManager::GetInstance(){
    if(!instance_){
        instance_ = new PSOManager();
    }

    return instance_;
}

PSOManager::~PSOManager(){}

void PSOManager::Release(){
    // 各PSOの解放
    for(auto& pso : psoDictionary_){
        for(auto& topology : pso.second.dict_){
            for(auto& cullMode : topology){
                for(auto& blendMode : cullMode){
                    blendMode.reset();
                }
            }
        }
    }

    // CS用のPSOの解放
    for(auto& csPso : csPsoDictionary_){
        csPso.second.reset();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// PSOの取得
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PSO* PSOManager::GetPSO(const std::string& name, BlendMode blendMode, D3D12_CULL_MODE cullMode, PolygonTopology topology){
    if(instance_->psoDictionary_.find(name) == instance_->psoDictionary_.end()){
        assert(false);// 存在しない or CS用のPSOを取得しようとした
    }

    return instance_->psoDictionary_[name].dict_[int(topology)][int(cullMode) - 1][int(blendMode)].get();
}

PSO* PSOManager::GetPSO_Compute(const std::string& name){
    if(instance_->csPsoDictionary_.find(name) == instance_->csPsoDictionary_.end()){
        assert(false);// 存在しない or 通常のPSOを取得しようとした
    }
    return instance_->csPsoDictionary_[name].get();
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// ファイルから自動でPSOを作成する関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PSOManager::CreatePipelines(const std::string& filename){

    // ファイルを開く
    static std::string directory = "Resources/Pipelines/";
    std::ifstream file(directory + filename);
    if(!file.is_open()){
        Log("File open error");
        return;
    }

    // .pipじゃないとエラー
    if(filename.substr(filename.find_last_of(".") + 1) != "pip"){
        Log("File format error");
        return;
    }

    // パイプラインの情報を格納する変数
    PipelineType pipelineType = PipelineType::VSPipeline;
    D3D12_COMPARISON_FUNC depthFunc = D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS_EQUAL;

    struct TmpShaderInfo{
        std::string name;
        IDxcBlob* blob;
        ID3D12ShaderReflection* reflection;
    };

    TmpShaderInfo vsInfo;
    TmpShaderInfo msInfo;
    TmpShaderInfo psInfo;
    TmpShaderInfo csInfo;
    TmpShaderInfo asInfo;
    std::array<TmpShaderInfo*, 5> shaderInfo = { &vsInfo, &msInfo ,&psInfo, &csInfo, &asInfo };

    // 1行ずつ読み込む
    std::string line;
    while(std::getline(file, line)){

        // "#"がなければスルー
        if(line.find("#") == std::string::npos){
            continue;
        }

        // "#1" を見つけた時: パイプラインの種類を決定
        if(line.find("#1") != std::string::npos){
            // = より後ろを取得
            std::string value = line.substr(line.find("=") + 1);
            // パイプラインの種類を決定
            pipelineType = static_cast<PipelineType>(std::stoi(value));
            continue;
        }

        // "#2" を見つけた時: DepthFuncを決定
        if(line.find("#2") != std::string::npos){
            // = より後ろを取得
            std::string value = line.substr(line.find("=") + 1);
            // DepthFuncを決定
            depthFunc = static_cast<D3D12_COMPARISON_FUNC>(std::stoi(value));
            continue;
        }

        // "#3" を見つけた時: VS名を取得
        if(line.find("#3") != std::string::npos){
            // = より後ろを取得
            std::string shaderName = line.substr(line.find("=") + 1);
            // 空白を削除
            shaderName.erase(std::remove_if(shaderName.begin(), shaderName.end(), isspace), shaderName.end());
            // シェーダー名を格納
            vsInfo.name = shaderName;
            // Blobの取得
            vsInfo.blob = ShaderDictionary::GetInstance()->GetShaderBlob(shaderName);
            // Reflectionの取得
            vsInfo.reflection = ShaderDictionary::GetInstance()->GetReflection(shaderName);
            continue;
        }

        // "#4" を見つけた時: MS名を取得
        if(line.find("#4") != std::string::npos){
            // = より後ろを取得
            std::string shaderName = line.substr(line.find("=") + 1);
            // 空白を削除
            shaderName.erase(std::remove_if(shaderName.begin(), shaderName.end(), isspace), shaderName.end());
            // シェーダー名を格納
            msInfo.name = shaderName;
            // Blobの取得
            msInfo.blob = ShaderDictionary::GetInstance()->GetShaderBlob(shaderName);
            // Reflectionの取得
            msInfo.reflection = ShaderDictionary::GetInstance()->GetReflection(shaderName);
            continue;
        }

        // "#5" を見つけた時: PS名を取得
        if(line.find("#5") != std::string::npos){
            // = より後ろを取得
            std::string shaderName = line.substr(line.find("=") + 1);
            // 空白を削除
            shaderName.erase(std::remove_if(shaderName.begin(), shaderName.end(), isspace), shaderName.end());
            // シェーダー名を格納
            psInfo.name = shaderName;
            // Blobの取得
            psInfo.blob = ShaderDictionary::GetInstance()->GetShaderBlob(shaderName);
            // Reflectionの取得
            psInfo.reflection = ShaderDictionary::GetInstance()->GetReflection(shaderName);
            continue;
        }

        // "#6" を見つけた時: CS名を取得
        if(line.find("#6") != std::string::npos){
            // = より後ろを取得
            std::string shaderName = line.substr(line.find("=") + 1);
            // 空白を削除
            shaderName.erase(std::remove_if(shaderName.begin(), shaderName.end(), isspace), shaderName.end());
            // シェーダー名を格納
            csInfo.name = shaderName;
            // Blobの取得
            csInfo.blob = ShaderDictionary::GetInstance()->GetShaderBlob(shaderName);
            // Reflectionの取得
            csInfo.reflection = ShaderDictionary::GetInstance()->GetReflection(shaderName);
            continue;
        }

        // "#7" を見つけた時: AS名を取得
        if(line.find("#7") != std::string::npos){
            // = より後ろを取得
            std::string shaderName = line.substr(line.find("=") + 1);
            // 空白を削除
            shaderName.erase(std::remove_if(shaderName.begin(), shaderName.end(), isspace), shaderName.end());
            // シェーダー名を格納
            asInfo.name = shaderName;
            // Blobの取得
            asInfo.blob = ShaderDictionary::GetInstance()->GetShaderBlob(shaderName);
            // Reflectionの取得
            asInfo.reflection = ShaderDictionary::GetInstance()->GetReflection(shaderName);
            continue;
        }
    }


    // PSOの作成
    if(pipelineType != PipelineType::CSPipeline){

        // 表示される系のパイプライン(CS以外)はblendmode,topology,cullmodeごとに作成
        for(int topology = 0; topology < kTopologyCount; topology++){
            for(int cullMode = 0; cullMode < kCullModeCount; cullMode++){
                for(int blendMode = 0; blendMode < kBlendModeCount; blendMode++){
                    
                    std::unique_ptr<PSO> pso = std::make_unique<PSO>();

                    // Pipelineの作成
                    if(pipelineType == PipelineType::VSPipeline){
                        pso->pipeline = std::make_unique<Pipeline>();

                    } else if(pipelineType == PipelineType::MSPipeline){
                        pso->pipeline = std::make_unique<MSPipeline>();
                    }

                    // 深度関数の設定
                    pso->pipeline.get()->depthFunc_ = depthFunc;

                    // シェーダーの設定
                    pso->pipeline.get()->pVsBlob_ = vsInfo.blob;
                    pso->pipeline.get()->pMsBlob_ = msInfo.blob;
                    pso->pipeline.get()->pPsBlob_ = psInfo.blob;
                    pso->pipeline.get()->pCsBlob_ = csInfo.blob;
                    pso->pipeline.get()->pAsBlob_ = asInfo.blob;

                    // 初期化
                    pso->pipeline.get()->Create(BlendMode(blendMode), PolygonTopology(topology), D3D12_CULL_MODE(cullMode + 1));


                    // RootSignatureの作成
                    pso->rootSignature = std::make_unique<RootSignature>();
                    for(int i = 0; i < shaderInfo.size(); i++){
                        if(shaderInfo[i]->name != ""){
                            // RootParameterの追加
                            instance_->GenerateRootParameters(pso->rootSignature.get(), shaderInfo[i]->name, shaderInfo[i]->reflection);
                        }

                        // VSのときだけInputLayoutを生成
                        if(i == 0){
                            instance_->GenerateInputLayout(
                                dynamic_cast<Pipeline*>(pso->pipeline.get()), shaderInfo[i]->reflection
                            );
                        }
                    }

                    // PSOの作成
                    Create(pso->rootSignature.get(), pso->pipeline.get(), false);

                    // 辞書に追加
                    instance_->psoDictionary_[filename].dict_[topology][cullMode][blendMode] = std::move(pso);
                }
            }
        }

    } else{// ComputeShaderのパイプラインの場合は一つだけ作成
        
        std::unique_ptr<PSO> pso = std::make_unique<PSO>();
        
        // RootSignatureの作成
        pso->rootSignature = std::make_unique<RootSignature>();

        // RootParameterの追加
        if(csInfo.name != ""){
            instance_->GenerateRootParameters(pso->rootSignature.get(), csInfo.name, csInfo.reflection);
        }

        // CSを設定する
        pso->pipeline = std::make_unique<Pipeline>();
        pso->pipeline.get()->pCsBlob_ = csInfo.blob;

        // PSOの作成
        Create(pso->rootSignature.get(), pso->pipeline.get(), true);

        // 辞書に追加
        instance_->csPsoDictionary_[filename] = std::move(pso);

    }

    file.close();
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// シェーダー情報からRootSignatureの情報を作成する関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PSOManager::GenerateRootParameters(
    RootSignature* rootSignature, const std::string& shaderName,
    ID3D12ShaderReflection* reflection
){
    if(!reflection){ return; }
    D3D12_SHADER_DESC shaderDesc = {};
    reflection->GetDesc(&shaderDesc);

    // シェーダーの種類を取得
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL;
    if(shaderName.find(".VS") != std::string::npos)      visibility = D3D12_SHADER_VISIBILITY_VERTEX;
    else if(shaderName.find(".MS") != std::string::npos) visibility = D3D12_SHADER_VISIBILITY_MESH;
    else if(shaderName.find(".PS") != std::string::npos) visibility = D3D12_SHADER_VISIBILITY_PIXEL;
    else if(shaderName.find(".CS") != std::string::npos) visibility = D3D12_SHADER_VISIBILITY_ALL; // CSはALLが必要
    else if(shaderName.find(".AS") != std::string::npos) visibility = D3D12_SHADER_VISIBILITY_ALL; // ASはALLが必要

    // 各リソースのバインディング情報を取得
    for(UINT i = 0; i < shaderDesc.BoundResources; ++i){

        // リフレクションからリソースのバインディング情報を取得
        D3D12_SHADER_INPUT_BIND_DESC bindDesc = {};
        reflection->GetResourceBindingDesc(i, &bindDesc);

        // パラメータの設定
        D3D12_ROOT_PARAMETER param = {};
        D3D12_DESCRIPTOR_RANGE_TYPE rangeType;
        param.ShaderVisibility = visibility;
        bool isConstant = false;

        if(bindDesc.Type == D3D_SIT_CBUFFER){
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// b
            rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
            isConstant = true;

        } else if(bindDesc.Type == D3D_SIT_STRUCTURED || bindDesc.Type == D3D_SIT_TEXTURE){
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;// t
            rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

        } else if(bindDesc.Type == D3D_SIT_UAV_RWTYPED){
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;// u
            rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

        } else{
            continue;// samplerやその他のタイプは無視
        }

        // パラメータやテーブルの追加
        if(!isConstant){
            rootSignature->AddParameter(
                std::string(bindDesc.Name),
                param.ParameterType,
                visibility,
                bindDesc.BindPoint,
                bindDesc.Space
            );
        } else{
            rootSignature->AddDescriptorTable(
                std::string(bindDesc.Name),
                rangeType,
                bindDesc.BindCount,
                bindDesc.BindPoint,
                visibility,
                bindDesc.Space
            );
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// シェーダー情報からInputLayoutを作成する関数
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PSOManager::GenerateInputLayout(Pipeline* pipeline, ID3D12ShaderReflection* reflection){

    if(!reflection){ return; }
    if(!pipeline){ return; }
    D3D12_SHADER_DESC shaderDesc = {};
    reflection->GetDesc(&shaderDesc);

    // 入力パラメータごとにループ
    for(UINT i = 0; i < shaderDesc.InputParameters; ++i){
        D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
        reflection->GetInputParameterDesc(i, &paramDesc);

        // セマンティク情報の取得
        std::string semanticName = paramDesc.SemanticName;
        UINT semanticIndex = paramDesc.SemanticIndex;
        UINT slot = 0;
        D3D12_INPUT_CLASSIFICATION inputClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;

        // セマンティク名から情報をマッチング
        std::regex re(R"(S(\d+)_(V|I)_([A-Z_]+))");
        std::smatch match;
        if(std::regex_match(semanticName, match, re)){
            slot = std::stoi(match[1].str());
            inputClass = (match[2].str() == "I") ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        
        } else{

            // "SV_"がついている場合は無視
            if(semanticName.find("SV_") != std::string::npos){
                continue;
            }

            /*
                セマンティク名は "スロット"_"VertexData(V)"or"InstanceData(I)"_"セマンティクス名 + セマンティクスインデックス"の形式にしてください
                例: S0_V_POSITION0 (slot0,VertexData,POSITION,セマンティク番号0)
            */
            assert(false);
        }

        // formatの取得
        DXGI_FORMAT format = GetFormatFromSignatureDesc(paramDesc);

        // InoutElementの追加
        pipeline->AddInputElementDesc(
            semanticName.c_str(), semanticIndex, slot, format, inputClass
        );
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// シグネチャの情報からFormatを取得する関数
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DXGI_FORMAT PSOManager::GetFormatFromSignatureDesc(const D3D12_SIGNATURE_PARAMETER_DESC& desc){
    UINT componentCount = 0;

    // Maskは下位4bitを使っていて、各ビットが1ならその分の要素数を示す
    switch(desc.Mask){
    case 0x1: componentCount = 1; break;  // float,intなど
    case 0x3: componentCount = 2; break;  // float2,int2など
    case 0x7: componentCount = 3; break;  // float3,int3など
    case 0xF: componentCount = 4; break;  // float4,int4など
    default: return DXGI_FORMAT_UNKNOWN;
    }

    // コンポーネントの型に応じてFormatを取得
    switch(desc.ComponentType){
    case D3D_REGISTER_COMPONENT_FLOAT32:
        switch(componentCount){
        case 1: return DXGI_FORMAT_R32_FLOAT;
        case 2: return DXGI_FORMAT_R32G32_FLOAT;
        case 3: return DXGI_FORMAT_R32G32B32_FLOAT;
        case 4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
        break;

    case D3D_REGISTER_COMPONENT_SINT32:
        switch(componentCount){
        case 1: return DXGI_FORMAT_R32_SINT;
        case 2: return DXGI_FORMAT_R32G32_SINT;
        case 3: return DXGI_FORMAT_R32G32B32_SINT;
        case 4: return DXGI_FORMAT_R32G32B32A32_SINT;
        }
        break;

    case D3D_REGISTER_COMPONENT_UINT32:
        switch(componentCount){
        case 1: return DXGI_FORMAT_R32_UINT;
        case 2: return DXGI_FORMAT_R32G32_UINT;
        case 3: return DXGI_FORMAT_R32G32B32_UINT;
        case 4: return DXGI_FORMAT_R32G32B32A32_UINT;
        }
        break;
    }

    return DXGI_FORMAT_UNKNOWN;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//      PSOの作成関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PSOManager::Create(RootSignature* pRootSignature, IPipeline* pPipeline, bool isCSPipeline){

    HRESULT hr;
    ComPtr<ID3DBlob> signatureBlob = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;

    /*-------------------- RootSignatureの作成 ----------------------*/

    // バイナリを生成
    hr = D3D12SerializeRootSignature(&pRootSignature->desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    // 失敗した場合
    if(FAILED(hr)){
        Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        assert(false);
    }

    // RootSignatureの作成
    hr = DxManager::GetInstance()->device->CreateRootSignature(
        0, signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(), IID_PPV_ARGS(pRootSignature->rootSignature.GetAddressOf())
    );

    // 失敗したらアサート
    assert(SUCCEEDED(hr));

    /*------------------------- PSOの作成 ---------------------------*/

    if(!isCSPipeline){
        // 型に応じた処理
        D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc{};

        if(MSPipeline* msPipeline = dynamic_cast<MSPipeline*>(pPipeline)){
            // descの設定
            msPipeline->pipelineDescs_.rootSignature = pRootSignature->rootSignature.Get();
            pipelineStateStreamDesc.SizeInBytes = sizeof(msPipeline->pipelineDescs_);
            pipelineStateStreamDesc.pPipelineStateSubobjectStream = &msPipeline->pipelineDescs_;

        } else if(Pipeline* pipeline = dynamic_cast<Pipeline*>(pPipeline)){
            // descの設定
            pipeline->pipelineDescs_.rootSignature = pRootSignature->rootSignature.Get();
            pipelineStateStreamDesc.SizeInBytes = sizeof(pipeline->pipelineDescs_);
            pipelineStateStreamDesc.pPipelineStateSubobjectStream = &pipeline->pipelineDescs_;
        } else{
            assert(false);
        }


        // 実際に生成
        hr = DxManager::GetInstance()->device->CreatePipelineState(
            &pipelineStateStreamDesc, IID_PPV_ARGS(pPipeline->pipeline_.GetAddressOf())
        );


    } else{
        // CS用のPSO作成設定
        D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.pRootSignature = pRootSignature->rootSignature.Get();
        psoDesc.CS = { reinterpret_cast<BYTE*>(pPipeline->pCsBlob_->GetBufferPointer()), pPipeline->pCsBlob_->GetBufferSize() };

        // 実際に生成
        hr = DxManager::GetInstance()->device->CreateComputePipelineState(
            &psoDesc, IID_PPV_ARGS(pPipeline->pipeline_.GetAddressOf())
        );
    }

    // 失敗したらアサート
    assert(SUCCEEDED(hr));

    /*---------------------------- 解放 ------------------------------*/

    signatureBlob->Release();;
    if(errorBlob){
        errorBlob->Release();
    }
}