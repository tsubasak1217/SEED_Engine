#pragma once
#include <SEED/Lib/Tensor/Vector2.h>

namespace SEED{
    namespace Topology{
        /// <summary>
        /// 円
        /// </summary>
        struct Circle{
            Vector2 center;
            float radius;
        };
    }
} // namespace SEED