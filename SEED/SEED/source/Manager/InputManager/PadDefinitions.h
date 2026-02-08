#pragma once
#include <cstdint>

namespace SEED{
    // パッドのボタンに名前と番号を割り当て
    enum class PAD_BUTTON : uint32_t{
        UP = 0b1,
        DOWN = 0b1 << 2,
        LEFT = 0b1 << 3,
        RIGHT = 0b1 << 4,
        START = 0b1 << 5,
        BACK = 0b1 << 6,
        L_STICK = 0b1 << 7,
        R_STICK = 0b1 << 8,
        LB = 0b1 << 9,
        RB = 0b1 << 10,
        A = 0b1 << 11,
        B = 0b1 << 12,
        X = 0b1 << 13,
        Y = 0b1 << 14,
        LT = 0b1 << 15,
        RT = 0b1 << 16
    };

    constexpr PAD_BUTTON operator|(PAD_BUTTON lhs, PAD_BUTTON rhs){
        return PAD_BUTTON(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
    }

    constexpr uint32_t operator&(PAD_BUTTON lhs, PAD_BUTTON rhs){
        return static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs);
    }

    // 比較演算子: PAD_BUTTON == uint32_t
    constexpr bool operator==(PAD_BUTTON lhs, uint32_t rhs){
        return static_cast<uint32_t>(lhs) == rhs;
    }

    constexpr bool operator==(uint32_t lhs, PAD_BUTTON rhs){
        return lhs == static_cast<uint32_t>(rhs);
    }

    // 比較演算子: PAD_BUTTON != uint32_t
    constexpr bool operator!=(PAD_BUTTON lhs, uint32_t rhs){
        return !(lhs == rhs);
    }

    constexpr bool operator!=(uint32_t lhs, PAD_BUTTON rhs){
        return !(lhs == rhs);
    }

    // パッドの状態(今か前か)
    enum class INPUT_STATE : BYTE{
        CURRENT = 0,// 今のフレームの状態を取得したいとき
        BEFORE = 1// 前のフレームの状態を取得したいとき
    };
}