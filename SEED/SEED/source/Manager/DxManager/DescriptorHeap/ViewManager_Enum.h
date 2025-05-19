#pragma once
#include <cstdint>

enum class HEAP_TYPE : int32_t{
    SRV_CBV_UAV = 0,
    RTV,
    DSV,
    CountOfHeapType
};

enum class VIEW_TYPE : int32_t{
    SRV = 0,
    CBV,
    UAV,
    RTV,
    DSV
};