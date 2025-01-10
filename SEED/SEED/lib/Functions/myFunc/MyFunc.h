#pragma once
#include <random>
#include <cstdint>
#include <map>
#include <vector>
#include <string>
#include "Vector3.h"
#include "Vector2.h"
#include "Range1D.h"
#include "Range3D.h"
#include "Range2D.h"


class MyFunc{
private:
    // 乱数生成器を作成
    static std::random_device rd; // staticに変更
    static std::mt19937 gen; // staticに変更

public:
    // 指定した範囲の乱数を返す関数
    static int32_t Random(int min,int max);
    static float Random(float min,float max);
    static Vector3 Random(const Vector3& min,const Vector3& max);
    static Vector3 Random(const Range3D& range);
    static Vector2 Random(const Vector2& min,const Vector2& max);
    static Vector2 Random(const Range2D& range);

    // ランダムな方向を返す関数
    static Vector3 RandomVector();
    static Vector2 RandomVector2();

    // thetaとphiからベクトルを生成する関数
    static Vector3 CreateVector(float theta,float phi);

    // 指定範囲を繰り返す関数 (最大値を超えたら最小値へ戻る)
    static int32_t Spiral(int32_t input,int32_t min,int32_t max);
    static float Spiral(float input,float min,float max);

    // ベクトルから三次元の回転角を算出する関数
    static Vector3 CalcRotateVec(const Vector3& vec);

    // マップのキーを検索して、特定のトークンを持つ要素を後ろに回す関数(何番目かも返す)
    template <typename T>
    static int32_t ToBack(std::map<std::string,T>& myMap,const std::string& token);

    /// <summary>
    /// ある時間 t(0~1) での放物線の位置を計算
    /// </summary>
    /// <param name="_sin">sin(theta)</param>
    /// <param name="_speed">初速</param>
    /// <param name="_time">時間 (0~1)</param>
    /// <param name="_gravity">重力加速度</param>
    /// <returns></returns>
    static Vector2 CalculateParabolic(float _sin,float _speed,float _time,float _gravity);
    /// <summary>
   /// ある時間 t(0~1) での放物線の位置を計算
   /// </summary>
   /// <param name="_direction">飛ぶ方向</param>
   /// <param name="speed">初速</param>
   /// <param name="_time">時間 (0~1)</param>
   /// <param name="_gravity">重力加速度</param>
   /// <returns></returns>
    static  Vector2 CalculateParabolic(const Vector2& _direction,float speed,float _time,float _gravity);
};

// マップのキーを検索して、特定のトークンを持つ要素を後ろに回す関数(何番目かも返す)
template<typename T>
inline int32_t MyFunc::ToBack(std::map<std::string,T>& myMap,const std::string& token){
    // 前後に分けるための一時コンテナ
    std::vector<std::pair<std::string,T>> withToken;
    std::vector<std::pair<std::string,T>> withoutToken;

    // 分ける処理
    for(const auto& [key,value] : myMap){
        if(key.find(token) != std::string::npos){
            withToken.push_back({key,value}); // トークンを含む
        } else{
            withoutToken.push_back({key,value}); // トークンを含まない
        }
    }

    // 元のマップをクリアして再挿入
    myMap.clear();
    for(const auto& pair : withoutToken){
        myMap.insert(pair);
    }
    for(const auto& pair : withToken){
        myMap.insert(pair);
    }

    // 何番目かを返す
    return withToken.size();
}