#include <SEED/Lib/Functions/RandomFunc.h>
#include <SEED/Lib/Functions/Math.h>
#include <SEED/Lib/Tensor/Quaternion.h>
#include <Windows.h>
#include <shobjidl.h>

// usiing
namespace fs = std::filesystem;

namespace SEED{
    namespace Methods{

        // staticメンバーの定義
        std::random_device Rand::rd;
        std::mt19937 Rand::gen(Rand::rd());

        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 指定した範囲の乱数を返す関数
        //////////////////////////////////////////////////////////////////////////////////////////////////////////

        //------------------ int ------------------//
        int32_t Rand::Random(int min, int max){

            // もしminがmaxより大きい場合は入れ替える
            if(min > max){ std::swap(min, max); }

            // minからmaxまでの一様分布を設定
            std::uniform_int_distribution<> distrib(min, max);

            // 乱数を生成して返す
            return distrib(gen);
        }

        //------------------ float ------------------//
        float Rand::Random(float min, float max){

            // もしminがmaxより大きい場合は入れ替える
            if(min > max){ std::swap(min, max); }

            // minからmaxまでの一様分布を設定 (float用)
            std::uniform_real_distribution<float> distrib(min, max);

            // 乱数を生成して返す
            return distrib(gen);
        }

        //------------------ uint64_t ------------------//
        uint64_t Rand::RandomU64(){
            int32_t hi = Methods::Rand::Random(0, INT32_MAX);
            int32_t lo = Methods::Rand::Random(0, INT32_MAX);

            // 64ビットに合成
            //  hi を上位32ビットに, lo を下位32ビットに詰める
            uint64_t high = static_cast<uint64_t>(hi);
            uint64_t low = static_cast<uint64_t>(lo);
            return (high << 32) | low;
        }

        //----------------- 3次元ベクトル用 -----------------//

        Vector3 Rand::Random(const Vector3& min, const Vector3& max){
            return { Methods::Rand::Random(min.x,max.x),Random(min.y,max.y),Random(min.z,max.z) };
        }

        Vector3 Rand::Random(const Range3D& range){
            return Methods::Rand::Random(range.min, range.max);
        }

        //----------------- 2次元ベクトル用 -----------------//

        Vector2 Rand::Random(const Vector2& min, const Vector2& max){
            return { Methods::Rand::Random(min.x,max.x),Random(min.y,max.y) };
        }

        Vector2 Rand::Random(const Range2D& range){
            return Methods::Rand::Random(range.min, range.max);
        }

        float Rand::Random(const Range1D& range){
            return Methods::Rand::Random(range.min, range.max);
        }



        //--------------- ランダムな方向を返す関数 ---------------//

        Vector3 Rand::RandomVector(){
            // ランダムなthetaとphiの範囲
            float theta = Rand::Random(0.0f, 2.0f * 3.14159265358979323846f); // 0 ~ 2π
            float phi = Rand::Random(0.0f, 3.14159265358979323846f); // 0 ~ π

            // 球座標から直交座標への変換
            float x = std::sin(phi) * std::cos(theta);
            float y = std::sin(phi) * std::sin(theta);
            float z = std::cos(phi);

            return { x,y,z };
        }

        Vector2 Rand::RandomVector2(){
            // ランダムなthetaの範囲
            float theta = Rand::Random(0.0f, 2.0f * 3.14159265358979323846f); // 0 ~ 2π

            // 球座標から直交座標への変換
            float x = std::cos(theta);
            float y = std::sin(theta);

            return { x,y };
        }

        //----------------- ランダムな色を返す関数 -----------------//
        Vector4 Rand::RandomColor(){
            return { Methods::Rand::Random(0.0f,1.0f),Random(0.0f,1.0f),Random(0.0f,1.0f),1.0f };
        }

        Vector4 Rand::RandomColor(std::initializer_list<uint32_t> colorList, bool isCorrectionToLiner){
            if(colorList.size() == 0){
                return { 1.0f,1.0f,1.0f,1.0f }; // デフォルトの白色
            }

            // ランダムに選択
            auto it = std::next(colorList.begin(), Methods::Rand::Random(0, static_cast<int>(colorList.size()) - 1));
            uint32_t color = *it;
            // RGBA成分を抽出して返す
            return Methods::Math::FloatColor(color, isCorrectionToLiner);
        }

        //----------------- ランダムな方向を返す関数 -----------------//
        Vector3 Rand::RandomDirection(const Vector3& baseDirection, float angle){
            // ランダムなthetaとphiの範囲
            float theta = Rand::Random(-angle, angle); // -angle ~ angle
            float phi = Rand::Random(-angle / 2.0f, angle / 2.0f); // -angle/2 ~ angle/2
            // 球座標から直交座標への変換
            float x = std::sin(phi) * std::cos(theta);
            float y = std::sin(phi) * std::sin(theta);
            float z = std::cos(phi);
            return baseDirection * Quaternion::DirectionToDirection({ 1.0f,0.0f,0.0f }, { x,y,z });
        }
    }
} // namespace SEED