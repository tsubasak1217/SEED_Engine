#pragma once
#include <cstdint>

namespace SEED{
    namespace GeneralEnum{
        /// <summary>
        /// 色の扱い方を定義
        /// </summary>
        enum ColorMode : int32_t{
            RGBA,
            HSVA
        };
    }
} // namespace SEED
