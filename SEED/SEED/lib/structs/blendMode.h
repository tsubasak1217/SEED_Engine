#pragma once

namespace SEED{

    // ブレンドモード
    enum class BlendMode{
        NONE = 0,
        MULTIPLY,
        SUBTRACT,
        NORMAL,
        ADD,
        SCREEN,
        kBlendModeCount
    };

} // namespace SEED