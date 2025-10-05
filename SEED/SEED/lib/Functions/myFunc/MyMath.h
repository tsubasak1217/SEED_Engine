#pragma once
#include <climits>
#include <stdint.h>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <SEED/Lib/Includes/include_tensors.h>
#include <SEED/Lib/Shapes/Line.h>

const float kDeltaTime = 0.016f;

enum VIEWMODE{
    kScreen,
    kWorld
};

class MyMath{
public:
    //================================================================
    //                      数学的な関数
    //================================================================

    /*---------------------------- 長さを計る関数 ----------------------------*/

    // 2点間の距離を計算する関数 (2D)
    static float Length(const Vector2& pos1,const Vector2& pos2);
    // 2点間の距離を計算する関数 (3D)
    static float Length(const Vector3& pos1,const Vector3& pos2);
    // ベクトルの長さを返す関数 (2D)
    static float Length(const Vector2& vec);
    // ベクトルの長さを返す関数 (3D)
    static float Length(const Vector3& vec);

    // 2点間の距離の2乗を計算する関数 (2D)
    static float LengthSq(const Vector2& pos1,const Vector2& pos2);
    // 2点間の距離の2乗を計算する関数 (3D)
    static float LengthSq(const Vector3& pos1,const Vector3& pos2);
    // ベクトルの長さの2乗を返す関数 (2D)
    static float LengthSq(const Vector2& vec);
    // ベクトルの長さの2乗を返す関数 (3D)
    static float LengthSq(const Vector3& vec);

    // ベクトルの長さが0かどうかを返す関数 (2D)
    static bool HasLength(const Vector2& vec);
    // ベクトルの長さが0かどうかを返す関数 (3D)
    static bool HasLength(const Vector3& vec);

    /*----------------------- ベクトルを正規化する関数 ------------------------*/

    // ベクトルを正規化する関数 (2D)
    static Vector2 Normalize(const Vector2& vec);
    // ベクトルを正規化する関数 (3D)
    static Vector3 Normalize(const Vector3& vec);

    /*- 仮: 非線形を線形に変換する関数-*/
    static float DepthToLinear(float depth,float near,float far);

    /*-------------------------- 内積を求める関数 ---------------------------*/

    // 2ベクトルの内積を求める関数 (2D)
    static float Dot(const Vector2& vec1,const Vector2& vec2);
    // 2ベクトルの内積を求める関数 (3D)
    static float Dot(const Vector3& vec1,const Vector3& vec2);
    // 3点の座標を指定して内積を求める関数 (2D)
    static float Dot(const Vector2& pos1,const Vector2& pos2,const Vector2& targetPos);
    // 3点の座標を指定して内積を求める関数 (3D)
    static float Dot(const Vector3& pos1,const Vector3& pos2,const Vector3& targetPos);
    // 2ベクトルの内積を求める関数 (長さで正規化する) (2D)
    static float DotNormal(const Vector2& vec1,const Vector2& vec2);
    // 2ベクトルの内積を求める関数 (長さで正規化する) (3D)
    static float DotNormal(const Vector3& vec1,const Vector3& vec2);
    // 3点の座標を指定して内積を求める関数 (長さで正規化する) (2D)
    static float DotNormal(const Vector2& pos1,const Vector2& pos2,const Vector2& targetPos);
    // 3点の座標を指定して内積を求める関数 (長さで正規化する) (3D)
    static float DotNormal(const Vector3& pos1,const Vector3& pos2,const Vector3& targetPos);

    /*-------------------------- 外積を求める関数 ---------------------------*/

    // 2ベクトルの外積を求める関数 (2D)
    static float Cross(const Vector2& vec1,const Vector2& vec2);
    // 2ベクトルの外積を求める関数 (3D)
    static Vector3 Cross(const Vector3& vec1,const Vector3& vec2,bool kViewMode = kWorld);
    // 3点の座標を指定して外積を求める関数 (長さで正規化する) (2D)
    static float Cross(const Vector2& originPos,const Vector2& endPos,const Vector2& targetPos);

    /*----------------------- 射影ベクトルを求める関数 -----------------------*/

    // 3点の座標を指定して射影ベクトルを求める関数 (2D)
    static Vector2 ProjectVec(const Vector2& pos1,const Vector2& pos2,const Vector2& targetPos);
    // 3点の座標を指定して射影ベクトルを求める関数 (3D)
    static Vector3 ProjectVec(const Vector3& pos1,const Vector3& pos2,const Vector3& targetPos);
    // 2ベクトルから射影ベクトルを求める関数 (2D)
    static Vector2 ProjectVec(const Vector2& vec1,const Vector2& vec2);
    // 2ベクトルから射影ベクトルを求める関数 (3D)
    static Vector3 ProjectVec(const Vector3& vec1,const Vector3& vec2);

    /*-------------------- 線への最近傍点を求めるを求める関数 ------------------*/

    // 直線への最近傍点を求める関数 (2D)
    static Vector2 ClosestPoint(const Vector2& seg_origin,const Vector2& seg_end,const Vector2& point);
    // 直線への最近傍点を求める関数 (3D)
    static Vector3 ClosestPoint(const Vector3& seg_origin,const Vector3& seg_end,const Vector3& point);
    // 線分同士の最近傍点を求める関数 (2D)
    static std::array<Vector2, 2> LineClosestPoints(const Line2D& l1, const Line2D& l2);

    // 2Dベクトルの角度を求める関数 (radian)
    static float GetTheta(const Vector2& dir);

    //================================================================
    //                      個人用な便利関数
    //================================================================

    // 負数を0に変換する関数 (int)
    static uint32_t negaZero(int32_t num);
    // 負数を0に変換する関数 (float)
    static float negaZero(float num);
    //degからradに変更する関数
    static float Deg2Rad(float deg);
    // tから要素数を算出する関数
    static int32_t CalcElement(float t, int32_t size);
    // tから区間内のtを算出する関数
    static float CalcSectionT(float t, int32_t size);

    /*-------------------- 線形補完をする関数 ------------------*/
    static Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t);
    static Vector3 Lerp(const Vector3& v1,const Vector3& v2,float t);
    static Vector4 Lerp(const Vector4& v1,const Vector4& v2,float t);
    static float Lerp(const float v1,const float v2,float t);
    static float LerpShortAngle(float a,float b,float t);

    // ベジェ曲線を求める関数
    static Vector3 Bezier(const Vector3& p0,const Vector3& p1,const Vector3& p2,float t);
    static Vector3 Bezier(const Vector3& p0,const Vector3& p1,const Vector3& p2,const Vector3& p3,float t);

    //スプライン補完をする関数
    static Vector3 CatmullRomInterpolation(const Vector3& p0,const Vector3& p1,const Vector3& p2,const Vector3& p3,float t);
    static Vector2 CatmullRomInterpolation(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3, float t);
    static float CatmullRomInterpolation(const float p0, const float p1, const float p2, const float p3, float t);
    //スプライン補完を使用して位置を設定する関数
    static Vector3 CatmullRomPosition(const std::vector<Vector3>& controlPoints,float t);
    static Vector3 CatmullRomPosition(const std::vector<Vector3*>& controlPoints,float t);
    static Vector2 CatmullRomPosition(const std::vector<Vector2>& controlPoints, float t);
    static float CatmullRomPosition(const std::vector<float>& controlPoints, float t);
    // 制御点を等間隔に修正する関数
    static void ToConstantControlPoints(std::vector<Vector3>* pControlPoints);
    static void ToConstantControlPoints(std::vector<Vector2>* pControlPoints);

    static Vector3 TransformNormal(const Vector3& normal,const Matrix4x4& matrix);

    //================================================================
    //                      型変換の関数
    //================================================================

    // floatをuint32_tに変換する関数
    static uint32_t FloatToUint32(float value);

    // uint32_tをfloatに変換する関数
    static float Uint32ToFloat(uint32_t value);

    //================================================================
    //                        色を扱う関数
    //================================================================

    /*-------------------- 色の各成分を求める関数 ------------------*/

    // RGBA形式のカラーコードからREDの成分を得る関数
    static uint32_t Red(uint32_t color);
    // RGBA形式のカラーコードからGREENの成分を得る関数
    static uint32_t Green(uint32_t color);
    // RGBA形式のカラーコードからBLUEの成分を得る関数
    static uint32_t Blue(uint32_t color);
    // RGBA形式のカラーコードからALPHA(透明度)の成分を得る関数
    static uint32_t Alpha(uint32_t color);

    // グレースケールを求める関数
    static uint32_t GrayScale(uint32_t color);

    // カラーコードからVector4に変換する関数 (0~1に収められる)
    static Vector4 FloatColor(uint32_t color,bool isCorrectionToLiner = true);
    static Vector4 FloatColor(uint32_t r,uint32_t g,uint32_t b,uint32_t a = 255, bool isCorrectionToLiner = true);
    // Vector4からカラーコードに変換する関数
    static uint32_t IntColor(const Vector4& color);
    static uint32_t IntColor(uint32_t r, uint32_t g, uint32_t b, uint32_t a = 255);

    // HSVをRGBに変換する関数
    static Vector4 HSV_to_RGB(float h, float s, float v, float alpha);
    static Vector4 HSV_to_RGB(const Vector4& HSVA_color);
    static Vector4 RGB_to_HSV(const Vector4& rgbColor);
    static Vector4 RGB_to_HSV(uint32_t colorCode);
};