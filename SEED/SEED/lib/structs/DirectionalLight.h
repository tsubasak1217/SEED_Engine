#pragma once
#include <SEED/Lib/Structs/BaseLight.h>

namespace SEED{

    /// <summary>
    /// 範囲を持たない一方向の平行光源
    /// </summary>
    struct DirectionalLight : public BaseLight{
        DirectionalLight();
        Vector3 direction_;
    };

} // namespace SEED