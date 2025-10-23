#pragma once
#include <cstdint>

/// <summary>
/// ヒープの管理しているviewの種類
/// </summary>
enum class HEAP_TYPE : int32_t{
    SRV_CBV_UAV = 0,
    RTV,
    DSV,
    CountOfHeapType
};

/// <summary>
/// ビューのタイプ
/// </summary>
enum class VIEW_TYPE : int32_t{
    SRV = 0,
    CBV,
    UAV,
    RTV,
    DSV
};