#include "MyFunc.h"
#include "MyMath.h"

// staticメンバーの定義
std::random_device MyFunc::rd;
std::mt19937 MyFunc::gen(MyFunc::rd());

int32_t MyFunc::Random(int min, int max){

    // minからmaxまでの一様分布を設定
    std::uniform_int_distribution<> distrib(min, max);

    // 乱数を生成して返す
    return distrib(gen);
}

float MyFunc::Random(float min, float max){

    // minからmaxまでの一様分布を設定 (float用)
    std::uniform_real_distribution<float> distrib(min, max);

    // 乱数を生成して返す
    return distrib(gen);
}


//
Vector3 MyFunc::Random(const Range3D& range){
    return {
        Random(range.min.x, range.max.x),
        Random(range.min.y, range.max.y),
        Random(range.min.z, range.max.z)
    };
}

Vector2 MyFunc::Random(const Range2D& range){
    return {
    Random(range.min.x, range.max.x),
    Random(range.min.y, range.max.y)
    };
}



int32_t MyFunc::Spiral(int32_t input,int32_t min, int32_t max){

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


float MyFunc::Spiral(float input,float min, float max){

    if(max <= min){
        assert(false);
    }

    float range = max - min;

    if(input > max){

        float sub = input - max;
        return min + std::fmod(sub,range);

    } else if(input < min){

        float sub = input - min;
        return max + std::fmod(sub,range);
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