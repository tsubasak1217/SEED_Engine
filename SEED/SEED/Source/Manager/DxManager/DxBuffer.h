#pragma once
#include "DxResource.h"

template<typename T>
struct DxBuffer{
    T* data;
    DxResource bufferResource;

public:
    // Mapする
    void Map(){
        bufferResource.resource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&data));
    }
};