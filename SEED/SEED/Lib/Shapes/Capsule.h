#pragma once
#include <SEED/Lib/Tensor/Vector3.h>

namespace SEED{
    namespace Topology{
        /// <summary>
        /// 3次元カプセル
        /// </summary>
        struct Capsule{
            Vector3 origin;
            Vector3 end;
            float radius;
        };

        /// <summary>
        /// 2次元カプセル
        /// </summary>
        struct Capsule2D{
            Vector2 origin;
            Vector2 end;
            float radius;
        };
    }
} // namespace SEED