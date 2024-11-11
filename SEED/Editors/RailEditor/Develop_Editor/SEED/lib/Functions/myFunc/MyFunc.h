#include <random>
#include <cstdint>
#include "Vector3.h"
#include "Vector2.h"
#include "Range3D.h"
#include "Range2D.h"


class MyFunc{
private:
    // 乱数生成器を作成
    static std::random_device rd; // staticに変更
    static std::mt19937 gen; // staticに変更

public:
    // 指定した範囲の乱数を返す関数
    static int32_t Random(int min, int max);
    static float Random(float min, float max);
    static Vector3 Random(const Range3D& range);
    static Vector2 Random(const Range2D& range);

    // 指定範囲を繰り返す関数 (最大値を超えたら最小値へ戻る)
    static int32_t Spiral(int32_t input,int32_t min, int32_t max);
    static float Spiral(float input,float min, float max);

    // ベクトルから三次元の回転角を算出する関数
    static Vector3 CalcRotateVec(const Vector3& vec);
};

