#pragma once
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Lib/Functions/MatrixMath.h>
#include <array>

namespace SEED{
    namespace Topology{
        /// <summary>
        /// 軸平行でないバウンディングボックス
        /// </summary>
        struct OBB{
            Vector3 center;
            Vector3 halfSize = { 1.0f,1.0f,1.0f };
            Vector3 rotate;

        public:
            std::array<Vector3, 8> GetVertices()const;
        };

        /// <summary>
        /// 軸平行でないバウンディングボックス(2D)
        /// </summary>
        struct OBB2D{
            Vector2 center;
            Vector2 halfSize = { 1.0f,1.0f };
            float rotate;

        public:
            std::array<Vector2, 4> GetVertices()const;
        };
    }
} // namespace SEED