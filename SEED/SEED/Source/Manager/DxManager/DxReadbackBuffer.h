#pragma once
#include "DxResource.h"
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>
#include <SEED/Source/Manager/DxManager/DxBuffer.h>

template<typename T>
struct DxReadbackBuffer{
    DxBuffer<T> gpuBuffer;

private:
    DxBuffer<T> readbackBuffer;// 書き込みはできないようにprivateにする

public:
    // バッファリソースの作成
    void CreateBuffer(ID3D12Device* device, size_t sizeInBytes, D3D12_RESOURCE_FLAGS resourceFlag){
        gpuBuffer.bufferResource.resource = CreateBufferResource(device, sizeInBytes, D3D12_HEAP_TYPE_DEFAULT, resourceFlag);
        readbackBuffer.bufferResource.resource = CreateBufferResource(device, sizeInBytes, D3D12_HEAP_TYPE_READBACK, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_DEST);
        readbackBuffer.Map();// 値を確認できるようにMapする
        readbackBuffer.bufferResource.InitState(D3D12_RESOURCE_STATE_COPY_DEST); // ReadbackBufferはコピー先として初期化
    }

    // ReadbackBufferにコピーする
    void CopyToReadbackBuffer(ID3D12GraphicsCommandList* commandList){

        // コピー可能なStateに変更
        gpuBuffer.bufferResource.TransitionState(D3D12_RESOURCE_STATE_COPY_SOURCE);
        readbackBuffer.bufferResource.TransitionState(D3D12_RESOURCE_STATE_COPY_DEST);

        // GPUバッファからリードバックバッファへコピー
        commandList->CopyResource(readbackBuffer.bufferResource.resource.Get(), gpuBuffer.bufferResource.resource.Get());

        // 前の状態に戻す
        gpuBuffer.bufferResource.RevertState();
        readbackBuffer.bufferResource.RevertState();
    }

    // state関連
    void InitState(D3D12_RESOURCE_STATES _state) {
        gpuBuffer.bufferResource.InitState(_state);
    }

    void TransitionState(D3D12_RESOURCE_STATES stateAfter) {
        gpuBuffer.bufferResource.TransitionState(stateAfter);
    }

    // viewの作成
    void CreateSRV(const std::string& viewName){
        gpuBuffer.bufferResource.CreateSRV(viewName);
    }

    void CreateUAV(const std::string& viewName){
        gpuBuffer.bufferResource.CreateUAV(viewName);
    }


    // 解放
    void Release() {
        gpuBuffer.Release();
        readbackBuffer.Release();
    }

public: // アクセッサ

    // 値の取得
    T GetValue() {
        if (readbackBuffer.data) {
            return *readbackBuffer.data; // ReadbackBufferからデータを取得
        }
        return T(); // データがない場合はデフォルト値を返す
    }

    // ポインタの取得
    T* GetDataPtr(){
        return readbackBuffer.data; // ReadbackBufferのデータポインタを返す
    }


    // virtualAddressの取得
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const{
        return gpuBuffer.bufferResource.resource->GetGPUVirtualAddress();
    }

    // handleの取得
    D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandle() const{
        return gpuBuffer.bufferResource.GetSRVHandle();
    }

    D3D12_GPU_DESCRIPTOR_HANDLE GetUAVHandle() const{
        return gpuBuffer.bufferResource.GetUAVHandle();
    }

    // 名前の取得
    const std::string& GetSRVName() const{
        return gpuBuffer.bufferResource.GetSRVName();
    }

    const std::string& GetUAVName() const{
        return gpuBuffer.bufferResource.GetUAVName();
    }

    // SRVのdescの取得
    D3D12_SHADER_RESOURCE_VIEW_DESC& GetSRVDesc() {
        return gpuBuffer.bufferResource.srvDesc;
    }

    // UAVのdescの取得
    D3D12_UNORDERED_ACCESS_VIEW_DESC& GetUAVDesc() {
        return gpuBuffer.bufferResource.uavDesc;
    }
};