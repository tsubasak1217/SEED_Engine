#include "MyFunc.h"
#include "MyMath.h"

// staticメンバーの定義
std::random_device MyFunc::rd;
std::mt19937 MyFunc::gen(MyFunc::rd());

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 指定した範囲の乱数を返す関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//------------------ int ------------------//
int32_t MyFunc::Random(int min, int max){

    // もしminがmaxより大きい場合は入れ替える
    if(min > max){ std::swap(min, max); }

    // minからmaxまでの一様分布を設定
    std::uniform_int_distribution<> distrib(min, max);

    // 乱数を生成して返す
    return distrib(gen);
}

//------------------ float ------------------//
float MyFunc::Random(float min, float max){

    // もしminがmaxより大きい場合は入れ替える
    if(min > max){std::swap(min, max);}

    // minからmaxまでの一様分布を設定 (float用)
    std::uniform_real_distribution<float> distrib(min, max);

    // 乱数を生成して返す
    return distrib(gen);
}


//----------------- 3次元ベクトル用 -----------------//

Vector3 MyFunc::Random(const Vector3& min, const Vector3& max){
    return { Random(min.x, max.x),Random(min.y, max.y),Random(min.z, max.z) };
}

Vector3 MyFunc::Random(const Range3D& range){
    return Random(range.min, range.max);
}

//----------------- 2次元ベクトル用 -----------------//

Vector2 MyFunc::Random(const Vector2& min, const Vector2& max){
    return { Random(min.x, max.x),Random(min.y, max.y) };
}

Vector2 MyFunc::Random(const Range2D& range){
    return Random(range.min, range.max);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 指定範囲を繰り返す関数 (最大値を超えたら最小値へ戻る)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t MyFunc::Spiral(int32_t input, int32_t min, int32_t max){

    if(max <= min){
        assert(false);
    }

    int32_t range = max - min;

    if(input > max){

        int32_t sub = input - max;
        return min + ((sub - 1) % range);

    } else if(input < min){

        int32_t sub = input - min;
        return max + ((sub + 1) % range);
    }

    return input;
}


float MyFunc::Spiral(float input, float min, float max){

    if(max <= min){
        assert(false);
    }

    float range = max - min;

    if(input > max){

        float sub = input - max;
        return min + std::fmod(sub, range);

    } else if(input < min){

        float sub = input - min;
        return max + std::fmod(sub, range);
    }

    return input;
}



// ベクトルから三次元の回転角を算出する関数
Vector3 MyFunc::CalcRotateVec(const Vector3& vec){
    Vector3 rotate = { 0.0f,0.0f,0.0f };

    // 移動ベクトルから回転角をを求め更新する
    if(MyMath::Length(vec) != 0.0){
        rotate = {
            std::atan2(-vec.y, MyMath::Length(vec)),// 縦方向の回転角(X軸回り)
            std::atan2(vec.x, vec.z),// 横方向の回転角(Y軸回り)
            0.0f
        };
    }

    return rotate;
}