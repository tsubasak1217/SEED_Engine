#pragma once
#include <stdint.h>

struct OffsetData{
    int32_t instanceOffset;
    int32_t meshOffset;
    int32_t jointIndexOffset;
    int32_t jointinterval;
    int32_t primitiveInterval;
};