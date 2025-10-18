#pragma once
#include "DxResource.h"
#include <SEED/Lib/Functions/DxFunc.h>

template<typename T>
struct DxBuffer{
    T* data;
    DxResource bufferResource;

public:
    // Mapする
    void Map(){
        bufferResource.resource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&data));
    }

    // バッファリソースの作成
    void CreateBuffer(ID3D12Device* device, size_t sizeInBytes, D3D12_HEAP_TYPE heapLocation, D3D12_RESOURCE_FLAGS resourceFlag){
        bufferResource.resource = CreateBufferResource(device, sizeInBytes,heapLocation, resourceFlag);
    }

    // 解放
    void Release() {
        if (bufferResource.resource) {
            bufferResource.resource.Reset();
            data = nullptr; // Mapしたデータもnullptrにする
        }
    }

    // State関連
    void InitState(D3D12_RESOURCE_STATES _state) {
        bufferResource.InitState(_state);
    }

    void TransitionState(D3D12_RESOURCE_STATES stateAfter) {
        bufferResource.TransitionState(stateAfter);
    }

    // viewの作成
    void CreateSRV(const std::string& viewName) {
        bufferResource.CreateSRV(viewName);
    }

    void CreateUAV(const std::string& viewName) {
        bufferResource.CreateUAV(viewName);
    }


public:// アクセッサ
    D3D12_SHADER_RESOURCE_VIEW_DESC& GetSRVDesc() {
        return bufferResource.srvDesc;
    }

    D3D12_UNORDERED_ACCESS_VIEW_DESC& GetUAVDesc() {
        return bufferResource.uavDesc;
    }

    // virtualAddressの取得
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
        return bufferResource.resource->GetGPUVirtualAddress();
    }

    // handleの取得
    D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandle() const {
        return bufferResource.GetSRVHandle();
    }

    D3D12_GPU_DESCRIPTOR_HANDLE GetUAVHandle() const {
        return bufferResource.GetUAVHandle();
    }

    // 名前の取得
    const std::string& GetSRVName() const {
        return bufferResource.GetSRVName();
    }

    const std::string& GetUAVName() const {
        return bufferResource.GetUAVName();
    }
};