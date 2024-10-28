#pragma once
#include <cstdint>
#include "DxManager.h"

class DescriptorHeap{

public:
    uint32_t kMaxViewCount_;
    uint32_t descriptorSize_;

private:
    ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
};