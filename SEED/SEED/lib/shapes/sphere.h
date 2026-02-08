#pragma once
#include <SEED/Lib/Tensor/Vector3.h>
#include <vector>

namespace SEED {
    namespace Topology{
        /// <summary>
        /// 球
        /// </summary>
        struct Sphere{
            Vector3 center;
            float radius;
        };
    }
} // namespace SEED