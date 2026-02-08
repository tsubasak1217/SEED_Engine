#pragma once
#include <SEED/Lib/Tensor/Vector3.h>

namespace SEED {
    namespace Topology{
        /// <summary>
        /// 3Dの輪っか
        /// </summary>
        struct Ring{
            Vector3 center_;
            float innerRadius_;
            float outerRadius_;

            Ring() = default;
            void Draw();
        };
    }
} // namespace SEED