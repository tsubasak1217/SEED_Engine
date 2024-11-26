#pragma once
// mine
#include <ModelData.h>
#include <Material.h>
#include <VertexData.h>

// external
#include <string>
#include <format>
#include <cassert>
#include <stdint.h>
#include <fstream>
#include <sstream>
#include <d3dx12.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <DirectXTex.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

class DxManager;

enum DX_RESOURCE_STATE : int{
    STATE_SHADER_RESOURCE,
    STATE_RENDER_TARGET,
    STATE_UNORDERED_ACCESS
};

// ログ関数
void Log(const std::string& message);
void Log(const std::wstring& message);

// 文字列を変換する関数
std::wstring ConvertString(const std::string& str);
std::string ConvertString(const std::wstring& str);

//
uint32_t Align(uint32_t size, uint32_t alignment);

// HLSLをコンパイルする関数
IDxcBlob* CompileShader(
    // CompilerするShaderファイルへのパス
    const std::wstring& filePath,
    // CompilerProfile
    const wchar_t* profile,
    const wchar_t* entryPoint,
    //初期化で生成したものを3つ
    IDxcUtils* dxcUtils,
    IDxcCompiler3* dxcCompiler,
    IDxcIncludeHandler* includeHandler
);

// DescriptorHeap作成関数
ID3D12DescriptorHeap* CreateDescriptorHeap(
    ID3D12Device* device,
    D3D12_DESCRIPTOR_HEAP_TYPE heapType, 
    UINT numDescriptors, 
    bool shaderVisible
);

void CreateDescriptorHeap(
    ID3D12Device* device,
    ID3D12DescriptorHeap* heap,
    D3D12_DESCRIPTOR_HEAP_TYPE heapType, 
    UINT numDescriptors,
    bool shaderVisible
);

// テクスチャを読み込む関数
DirectX::ScratchImage LoadTextureImage(const std::string& filePath);
ID3D12Resource* InitializeTextureResource(ID3D12Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format, DX_RESOURCE_STATE state);

// Resource作成関数
ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);
ID3D12Resource* CreateTextureResource(ID3D12Device* device,const DirectX::TexMetadata& metadata);
ID3D12Resource* CreateRenderTargetTextureResource(ID3D12Device* device, int32_t width, int32_t height);
ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);

// TextureResourceにデータを転送する関数
[[nodiscard]]
ID3D12Resource* UploadTextureData(
    ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device,
    ID3D12GraphicsCommandList* commandList);

// ディスクリプタのハンドルを取得する関数
D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
    ID3D12DescriptorHeap* descriptorHeap,
    uint32_t descriptorSize,
    uint32_t index);

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
    ID3D12DescriptorHeap* descriptorHeap,
    uint32_t descriptorSize,
    uint32_t index);

// Matrix4x4 を DirectX::XMMATRIX に変換する関数
DirectX::XMMATRIX ConvertToXMMATRIX(const Matrix4x4& matrix);