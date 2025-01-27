#include "MyFunc.h"
#include "MyMath.h"
#include "Quaternion.h"

// staticメンバーの定義
std::random_device MyFunc::rd;
std::mt19937 MyFunc::gen(MyFunc::rd());

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 指定した範囲の乱数を返す関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//------------------ int ------------------//
int32_t MyFunc::Random(int min,int max){

    // もしminがmaxより大きい場合は入れ替える
    if(min > max){ std::swap(min,max); }

    // minからmaxまでの一様分布を設定
    std::uniform_int_distribution<> distrib(min,max);

    // 乱数を生成して返す
    return distrib(gen);
}

//------------------ float ------------------//
float MyFunc::Random(float min,float max){

    // もしminがmaxより大きい場合は入れ替える
    if(min > max){ std::swap(min,max); }

    // minからmaxまでの一様分布を設定 (float用)
    std::uniform_real_distribution<float> distrib(min,max);

    // 乱数を生成して返す
    return distrib(gen);
}


//----------------- 3次元ベクトル用 -----------------//

Vector3 MyFunc::Random(const Vector3& min,const Vector3& max){
    return {Random(min.x,max.x),Random(min.y,max.y),Random(min.z,max.z)};
}

Vector3 MyFunc::Random(const Range3D& range){
    return Random(range.min,range.max);
}

//----------------- 2次元ベクトル用 -----------------//

Vector2 MyFunc::Random(const Vector2& min,const Vector2& max){
    return {Random(min.x,max.x),Random(min.y,max.y)};
}

Vector2 MyFunc::Random(const Range2D& range){
    return Random(range.min,range.max);
}

//--------------- ランダムな方向を返す関数 ---------------//

Vector3 MyFunc::RandomVector(){
    // ランダムなthetaとphiの範囲
    float theta = MyFunc::Random(0.0f,2.0f * 3.14159265358979323846f); // 0 ~ 2π
    float phi = MyFunc::Random(0.0f,3.14159265358979323846f); // 0 ~ π

    // 球座標から直交座標への変換
    float x = std::sin(phi) * std::cos(theta);
    float y = std::sin(phi) * std::sin(theta);
    float z = std::cos(phi);

    return {x,y,z};
}

Vector2 MyFunc::RandomVector2(){
    // ランダムなthetaの範囲
    float theta = MyFunc::Random(0.0f,2.0f * 3.14159265358979323846f); // 0 ~ 2π

    // 球座標から直交座標への変換
    float x = std::cos(theta);
    float y = std::sin(theta);

    return {x,y};
}

//----------------- ランダムな色を返す関数 -----------------//
Vector4 MyFunc::RandomColor() {
    return { Random(0.0f,1.0f),Random(0.0f,1.0f),Random(0.0f,1.0f),1.0f };
}

//----------------- ランダムな方向を返す関数 -----------------//
Vector3 MyFunc::RandomDirection(const Vector3& baseDirection, float angle) {
    // ランダムなthetaとphiの範囲
    float theta = MyFunc::Random(-angle, angle); // -angle ~ angle
    float phi = MyFunc::Random(-angle / 2.0f, angle / 2.0f); // -angle/2 ~ angle/2
    // 球座標から直交座標への変換
    float x = std::sin(phi) * std::cos(theta);
    float y = std::sin(phi) * std::sin(theta);
    float z = std::cos(phi);
    return baseDirection * Quaternion::DirectionToDirection({ 1.0f,0.0f,0.0f }, { x,y,z });
}

//----------------- thetaとphiからベクトルを生成する関数 -----------------//
Vector3 MyFunc::CreateVector(float theta,float phi){
    // 球座標から直交座標への変換（左手座標系用）
    float x = std::sin(phi) * std::cos(theta);
    float y = std::cos(phi);
    float z = std::sin(phi) * std::sin(theta);

    return {x,y,z};
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 指定範囲を繰り返す関数 (最大値を超えたら最小値へ戻る)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t MyFunc::Spiral(int32_t input,int32_t min,int32_t max){

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


float MyFunc::Spiral(float input,float min,float max){

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
    Vector3 rotate = {0.0f,0.0f,0.0f};

    // ベクトルの長さを計算
    float length = MyMath::Length(vec);
    if(length != 0.0f){
        // 正規化されたベクトル
        Vector3 normalizedVec = vec / length;

        // X軸回りの回転角
        rotate.x = -std::asin(normalizedVec.y); // Y成分で縦方向の角度を決定

        // Y軸回りの回転角
        rotate.y = std::atan2(normalizedVec.x,normalizedVec.z); // XとZの比率で横方向の角度を決定
    }

    return rotate;
}

Vector2 MyFunc::CalculateParabolic(const Vector2& _direction,float _speed,float _time,float _gravity){
    float x = _direction.x * _speed * _time;
    float y = _direction.y * _speed * _time - 0.5f * _gravity * _time * _time;
    return Vector2(x,y);
}