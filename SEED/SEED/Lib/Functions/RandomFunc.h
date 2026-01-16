#pragma once
#include <random>
#include <cstdint>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <initializer_list>
#include <json.hpp>
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Lib/Structs/Range1D.h>
#include <SEED/Lib/Structs/Range3D.h>
#include <SEED/Lib/Structs/Range2D.h>
#include <SEED/Lib/Structs/Transform.h>

namespace SEED{
    namespace Methods{
        // 乱数生成器を隠すため、関数クラス化
        class Rand{
        private:
            // 作成禁止
            Rand();
            Rand(const Rand&) = delete;

            // 乱数生成器
            static std::random_device rd;
            static std::mt19937 gen;

        public:
            // 指定した範囲の乱数を返す関数
            static int32_t Random(int min, int max);
            static float Random(float min, float max);
            static Vector3 Random(const Vector3& min, const Vector3& max);
            static Vector3 Random(const Range3D& range);
            static Vector2 Random(const Vector2& min, const Vector2& max);
            static Vector2 Random(const Range2D& range);
            static float Random(const Range1D& range);

            // 64ビットの乱数を返す関数
            static uint64_t RandomU64();

            // ランダムな方向を返す関数
            static Vector3 RandomVector();
            static Vector2 RandomVector2();

            // ランダムな色
            static Vector4 RandomColor();
            static Vector4 RandomColor(std::initializer_list<uint32_t>colorList, bool isCorrectionToLiner = true);

            // ランダムな方向(基準方向と許容角度から)
            static Vector3 RandomDirection(const Vector3& baseDirection, float angle);
        };
    }
} // namespace SEED