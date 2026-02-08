#pragma once
#include <string>
#include <SEED/Lib/Functions/DxFunc.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

namespace SEED{
    /// <summary>
    /// ID3D12ResourceのView作成やTransitionを簡単かつ安全に行うための構造体
    /// </summary>
    struct DxResource{
        // リソースの種類
        ComPtr<ID3D12Resource> resource;

        // desc
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = D3D12_SHADER_RESOURCE_VIEW_DESC{};
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = D3D12_UNORDERED_ACCESS_VIEW_DESC{};

        // stateの変更を行うための関数
        void TransitionState(D3D12_RESOURCE_STATES stateAfter);
        void RevertState();
        void InitState(D3D12_RESOURCE_STATES _state);

        // viewの作成
        void CreateSRV(const std::string& viewName);
        void CreateUAV(const std::string& viewName);

    public:
        // viewのindexの取得
        int32_t GetSRVIndex() const{
            return srvIndex;
        }

        int32_t GetUAVIndex() const{
            return uavIndex;
        }

        // viewのハンドルの取得
        D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandle() const{
            return srvHandle;
        }

        D3D12_GPU_DESCRIPTOR_HANDLE GetUAVHandle() const{
            return uavHandle;
        }

        // viewの名前の取得
        const std::string& GetSRVName() const{
            return srvName;
        }

        const std::string& GetUAVName() const{
            return uavName;
        }

    private:
        // リソースの情報
        D3D12_RESOURCE_STATES state;
        D3D12_RESOURCE_STATES stateBefore;

        // 名前
        std::string srvName; // SRVの名前
        std::string uavName; // UAVの名前

        // handle
        int32_t srvIndex = -1;
        int32_t uavIndex = -1;
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE uavHandle;
    };
}