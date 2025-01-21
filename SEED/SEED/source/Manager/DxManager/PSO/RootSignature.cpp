#include "RootSignature.h"

////////////////////////////////////////////////////////////
/// コンストラクタ (基本的な初期設定のみ)
////////////////////////////////////////////////////////////
RootSignature::RootSignature(){
    // 初期化
    samplerCount = 0;
    rangeCount = 0;
    parameterCount = 0;

    // sampler
    if(samplerCount < samplers.size()) {
        samplers[samplerCount].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT; // ポイントサンプリング(1箇所だけサンプリングする)
        samplers[samplerCount].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
        samplers[samplerCount].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplers[samplerCount].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplers[samplerCount].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
        samplers[samplerCount].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
        samplers[samplerCount].ShaderRegister = 0; // レジスタ番号s0を使う
        samplers[samplerCount].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // すべてのシェーダー共通で使う
        samplerCount++;
    }

    // descに情報設定
    desc.NumStaticSamplers = samplerCount;
    desc.pStaticSamplers = (samplerCount > 0) ? &samplers[0] : nullptr; // Null でない場合のみ有効
    desc.pParameters = (parameterCount > 0) ? &parameters[0] : nullptr;
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
}

////////////////////////////////////////////////////////////
/// ルートパラメーターを追加する
////////////////////////////////////////////////////////////
void RootSignature::AddParameter(
    D3D12_ROOT_PARAMETER_TYPE type,
    D3D12_SHADER_VISIBILITY visibility,
    UINT shaderRegister,
    UINT registerSpace,
    UINT num32BitValues
){
    if(parameterCount < parameters.size()) {
        // パラメーターの追加
        parameters[parameterCount].ParameterType = type;
        parameters[parameterCount].ShaderVisibility = visibility;
        parameters[parameterCount].Descriptor.ShaderRegister = shaderRegister;
        parameters[parameterCount].Descriptor.RegisterSpace = registerSpace;
        parameters[parameterCount].Constants.Num32BitValues = num32BitValues;
        parameters[parameterCount].Constants.ShaderRegister = shaderRegister;
        parameterCount++;

        // descにパラメーター情報設定
        desc.NumParameters = parameterCount;
        desc.pParameters = (parameterCount > 0) ? &parameters[0] : nullptr;
    } else {
        //要素数オーバー
        assert(false);
    }
}

////////////////////////////////////////////////////////////
/// ディスクリプタテーブルを追加する (パラメーターも併せて)
////////////////////////////////////////////////////////////
void RootSignature::AddDescriptorTable(
    D3D12_DESCRIPTOR_RANGE_TYPE type,
    UINT numDescriptors,
    UINT baseShaderRegister,
    D3D12_SHADER_VISIBILITY visibility,
    UINT registerSpace
){
    if(rangeCount < ranges.size()) {
        // レンジの設定
        ranges[rangeCount].RangeType = type;
        ranges[rangeCount].NumDescriptors = numDescriptors;
        ranges[rangeCount].BaseShaderRegister = baseShaderRegister;
        ranges[rangeCount].RegisterSpace = registerSpace;
        ranges[rangeCount].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        // テーブルの追加
        parameters[parameterCount].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        parameters[parameterCount].ShaderVisibility = visibility;
        parameters[parameterCount].DescriptorTable.NumDescriptorRanges = 1;
        parameters[parameterCount].DescriptorTable.pDescriptorRanges = &ranges[rangeCount];

        // インクリメント
        rangeCount++;
        parameterCount++;

        // descにパラメーター情報設定
        desc.NumParameters = parameterCount;
        desc.pParameters = (parameterCount > 0) ? &parameters[0] : nullptr;

    } else {
        //要素数オーバー
        assert(false);
    }
}

void RootSignature::Release(){
    rootSignature.Reset();
}
