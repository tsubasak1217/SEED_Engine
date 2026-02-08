#pragma once
#include <cstdint>

namespace SEED {
    namespace GeneralEnum{
        /// <summary>
        ///左右
        /// </summary>
        enum class LR : int32_t{
            NONE = -1,
            LEFT,
            RIGHT
        };

        /// <summary>
        /// 上下
        /// </summary>
        enum class UpDown : int32_t{
            NONE = -1,
            UP,
            DOWN
        };

        /// <summary>
        /// 上下左右
        /// </summary>
        enum class DIRECTION4 : int32_t{
            NONE = -1,
            UP,
            DOWN,
            LEFT,
            RIGHT
        };

        /// <summary>
        /// 8方向
        /// </summary>
        enum class DIRECTION8 : int32_t{
            NONE = -1,
            TOP,
            BOTTOM,
            LEFT,
            RIGHT,
            LEFTTOP,
            LEFTBOTTOM,
            RIGHTTOP,
            RIGHTBOTTOM
        };
    }
} // namespace SEED