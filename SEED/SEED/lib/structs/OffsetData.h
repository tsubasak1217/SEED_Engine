#pragma once
#include <stdint.h>

namespace SEED{

    /// <summary>
    /// 描画に使用するオフセット情報の構造体
    /// </summary>
    struct OffsetData{
        int32_t instanceOffset;
        int32_t meshOffset;
        int32_t jointIndexOffset;
        int32_t jointinterval;
        int32_t primitiveInterval;
    };

} // namespace SEED