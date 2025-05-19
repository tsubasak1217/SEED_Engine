#pragma once
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct DxResource{
    // リソースの種類
    ComPtr<ID3D12Resource> resource;

    // stateの変更を行うための関数
    void TransitionState(D3D12_RESOURCE_STATES stateAfter);
    void InitState(D3D12_RESOURCE_STATES _state);

private:
    // リソースの情報
    D3D12_RESOURCE_STATES state;
    D3D12_RESOURCE_STATES stateBefore;
};