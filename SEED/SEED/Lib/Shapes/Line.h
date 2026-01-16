#pragma once
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <stdint.h>

namespace SEED{
    namespace Topology{
        // 線の種類
        enum LINE_TYPE{
            SEGMENT = 0,
            RAY,
            LINE
        };

        /// <summary>
        /// 三次元の線
        /// </summary>
        struct Line{
            Vector3 origin_;
            Vector3 end_;
            int type_ = SEGMENT;

            Line() = default;
            Line(const Vector3& origin, const Vector3& end, int type = SEGMENT){
                origin_ = origin;
                end_ = end;
                type_ = type;
            }
        };

        /// <summary>
        /// 二次元の線
        /// </summary>
        struct Line2D{
            Vector2 origin_;
            Vector2 end_;
            int type_ = SEGMENT;
            Line2D() = default;
            Line2D(const Vector2& origin, const Vector2& end, int type = SEGMENT){
                origin_ = origin;
                end_ = end;
                type_ = type;
            }
        };
    }
} // namespace SEED