#include "MyMath.h"
#include <cmath>
#include <assert.h>
#include <numbers>

//================================================================
//                      数学的な関数
//================================================================

/*---------------------------- 長さを計る関数 ----------------------------*/

float MyMath::Length(const Vector2& pos1, const Vector2& pos2){
    float xLength = (pos1.x - pos2.x);
    float yLength = (pos1.y - pos2.y);
    return std::sqrt(xLength * xLength + yLength * yLength);
}

float MyMath::Length(const Vector3& pos1, const Vector3& pos2){
    float xLength = (pos1.x - pos2.x);
    float yLength = (pos1.y - pos2.y);
    float zLength = (pos1.z - pos2.z);
    return std::sqrt(xLength * xLength + yLength * yLength + zLength * zLength);
}

float MyMath::Length(const Vector2& vec){
    return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}

float MyMath::Length(const Vector3& vec){
    return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

float MyMath::LengthSq(const Vector2& pos1, const Vector2& pos2){
    float xLength = (pos1.x - pos2.x);
    float yLength = (pos1.y - pos2.y);
    return xLength * xLength + yLength * yLength;
}

float MyMath::LengthSq(const Vector3& pos1, const Vector3& pos2){
    float xLength = (pos1.x - pos2.x);
    float yLength = (pos1.y - pos2.y);
    float zLength = (pos1.z - pos2.z);
    return xLength * xLength + yLength * yLength + zLength * zLength;
}

float MyMath::LengthSq(const Vector2& vec){
    return vec.x * vec.x + vec.y * vec.y;
}

float MyMath::LengthSq(const Vector3& vec){
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

/*----------------------- ベクトルを正規化する関数 ------------------------*/

Vector2 MyMath::Normalize(const Vector2& vec){
    float length = Length(vec);
    return length != 0 ? vec / Length(vec) : Vector2(0.0f, 0.0f);
}

Vector3 MyMath::Normalize(const Vector3& vec){
    float length = Length(vec);
    return length != 0 ? vec / Length(vec) : Vector3(0.0f, 0.0f, 0.0f);
}


/*- 仮: 非線形を線形に変換する関数-*/
float MyMath::DepthToLinear(float depth, float near, float far){
    float z = depth * 2.0f - 1.0f;
    float linearDepth = (2.0f * near * far) / (far + near - z * (far - near));
    return (linearDepth - near) / (far - near);
}

/*-------------------------- 内積を求める関数 ---------------------------*/


float MyMath::Dot(const Vector2& a, const Vector2& b){
    return (a.x * b.x) + (a.y * b.y);
}

float MyMath::Dot(const Vector3& a, const Vector3& b){
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float MyMath::Dot(const Vector2& pos1, const Vector2& pos2, const Vector2& targetPos){
    return Dot(pos2 - pos1, targetPos - pos1);
}

float MyMath::Dot(const Vector3& pos1, const Vector3& pos2, const Vector3& targetPos){
    return Dot(pos2 - pos1, targetPos - pos1);
}


float MyMath::DotNormal(const Vector2& vec1, const Vector2& vec2){
    float length = Length(vec1);
    return length != 0.0f ? Dot(vec1, vec2) / length : 0.0f;
}

float MyMath::DotNormal(const Vector3& vec1, const Vector3& vec2){
    float length = Length(vec1);
    return length != 0.0f ? Dot(vec1, vec2) / length : 0.0f;
}

float MyMath::DotNormal(const Vector2& pos1, const Vector2& pos2, const Vector2& targetPos){
    Vector2 lineVector = pos2 - pos1;
    Vector2 forTarget = targetPos - pos1;
    float lineLength = Length(lineVector);

    return lineLength != 0 ? Dot(lineVector, forTarget) / lineLength : 0.0f;
}


float MyMath::DotNormal(const Vector3& pos1, const Vector3& pos2, const Vector3& targetPos){
    Vector3 lineVector = pos2 - pos1;
    Vector3 forTarget = targetPos - pos1;
    float lineLength = Length(lineVector);

    return lineLength != 0 ? Dot(lineVector, forTarget) / lineLength : 0.0f;
}

/*-------------------------- 外積を求める関数 ---------------------------*/

float MyMath::Cross(const Vector2& vec1, const Vector2& vec2){
    return vec1.x * vec2.y - vec1.y * vec2.x;
}

Vector3 MyMath::Cross(const Vector3& vec1, const Vector3& vec2, bool kViewMode){
    if(kViewMode == kScreen){
        return Vector3(
            -vec1.y * vec2.z - vec1.z * -vec2.y,
            vec1.z * vec2.x - vec1.x * vec2.z,
            vec1.x * -vec2.y - -vec1.y * vec2.x
        );
    }

    return Vector3(
        vec1.y * vec2.z - vec1.z * vec2.y,
        vec1.z * vec2.x - vec1.x * vec2.z,
        vec1.x * vec2.y - vec1.y * vec2.x
    );
}

float MyMath::Cross(const Vector2& originPos, const Vector2& endPos, const Vector2& targetPos){
    float length = Length(endPos - originPos);
    return length != 0 ? Cross(endPos - originPos, targetPos - originPos) / length : 0.0f;
}

/*---------------------- 射影ベクトルを求める関数 -----------------------*/

Vector2 MyMath::ProjectVec(const Vector2& pos1, const Vector2& pos2, const Vector2& targetPos){
    Vector2 vec = pos2 - pos1;
    return vec * DotNormal(pos1, pos2, targetPos);
}

Vector3 MyMath::ProjectVec(const Vector3& pos1, const Vector3& pos2, const Vector3& targetPos){
    Vector3 vec = pos2 - pos1;
    return vec * DotNormal(pos1, pos2, targetPos);
}

Vector2 MyMath::ProjectVec(const Vector2& vec1, const Vector2& vec2){
    return vec1 * DotNormal(vec1, vec2);
}

Vector3 MyMath::ProjectVec(const Vector3& vec1, const Vector3& vec2){
    return vec1 * DotNormal(vec1, vec2);
}

/*---------------------- 直線に対する最近傍点を求める関数 -----------------------*/

Vector2 MyMath::ClosestPoint(const Vector2& seg_origin, const Vector2& seg_end, const Vector2& point){
    float len = Length(seg_end - seg_origin);
    Vector2 proj = ProjectVec(seg_origin, seg_end, point);
    float projLen = Length(proj);

    if(projLen > len){
        return seg_end;
    } else if(projLen < 0){
        return seg_origin;
    }

    return seg_origin + proj;
}

Vector3 MyMath::ClosestPoint(const Vector3& seg_origin, const Vector3& seg_end, const Vector3& point){
    float len = Length(seg_end - seg_origin);
    Vector3 proj = ProjectVec(seg_origin, seg_end, point);
    float projLen = Length(proj);

    if(projLen > len){
        return seg_end;
    } else if(projLen < 0){
        return seg_origin;
    }

    return seg_origin + proj;
}


//================================================================
//                      個人用な便利関数
//================================================================

// 負数を0にする関数
uint32_t MyMath::negaZero(int32_t num){ return num > 0 ? num : 0; }
float MyMath::negaZero(float num){ return num > 0.0f ? num : 0.0f; }

float MyMath::Deg2Rad(float deg){
    return deg * (static_cast<float>(std::numbers::pi) / 180.0f);
}


float MyMath::Lerp(const float v1, const float v2, float t){
    return v1 + (v2 - v1) * t;
}

float MyMath::LerpShortAngle(float a, float b, float t){
    const float TWO_PI = 2.0f * (float)std::numbers::pi; // 2π (6.283185307179586)
    const float PI = (float)std::numbers::pi;            // π (3.141592653589793)

    // 角度差分を求める
    float diff = b - a;

    // 角度を[-π, π]に補正する
    diff = fmod(diff, TWO_PI);
    if(diff > PI){
        diff -= TWO_PI;
    } else if(diff < -PI){
        diff += TWO_PI;
    }

    // Lerpを使用して補間
    return Lerp(a, a + diff, t);

}

Vector3 MyMath::Bezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, float t){
    return Lerp(Lerp(p0, p1, t), Lerp(p1, p2, t), t);
}

Vector3 MyMath::Bezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t){
    return Lerp(Lerp(Lerp(p0, p1, t), Lerp(p1, p2, t), t), Lerp(Lerp(p1, p2, t), Lerp(p2, p3, t), t), t);
}

Vector3 MyMath::Lerp(const Vector3& v1, const Vector3& v2, float t){
    return v1 + (v2 - v1) * t;
}

Vector4 MyMath::Lerp(const Vector4& v1,const Vector4& v2,float t){
    return v1 + (v2 - v1) * t;
}

//================================================================
//                      スプライン補間の関数
//================================================================

// 4点を直接指定してCatmull-Rom補間を行う関数
Vector3 MyMath::CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t){

    t = std::clamp(t, 0.0f, 1.0f);// tを0~1に収める
    if(t <= 0.0f){
        return p0;
    } else if(t >= 1.0f){
        return p3;
    }
    float t2 = t * t;
    float t3 = t2 * t;

    Vector3 e3 = (p3 * 1) + (p2 * -3) + (p1 * 3) + (p0 * -1);
    Vector3 e2 = (p3 * -1) + (p2 * 4) + (p1 * -5) + (p0 * 2);
    Vector3 e1 = (p2 * 1) + (p0 * -1);
    Vector3 e0 = p1 * 2;

    return ((e3 * t3) + (e2 * t2) + (e1 * t) + e0) * 0.5f;
}

// 自由な数の制御点からCatmull-Rom補間を行い、tの地点を返す関数
Vector3 MyMath::CatmullRomPosition(const std::vector<Vector3>& controlPoints, float t){

    if(controlPoints.size() == 0){ return{ 0.0f,0.0f,0.0f }; }
    t = std::clamp(t, 0.0f, 1.0f);// tを0~1に収める

    Vector3 result;
    std::vector<Vector3> tmpControlPoints = controlPoints;
    // 要素数が必要数に達するまでコピーして追加
    while(tmpControlPoints.size() < 4){
        // 要素数が必要数に達するまでコピーして追加
        while(tmpControlPoints.size() < 4){
            tmpControlPoints.push_back(tmpControlPoints.back());
        }

        // 等間隔に制御点を修正
        ToConstantControlPoints(&tmpControlPoints);
    }

    int size = int(tmpControlPoints.size() - 1);
    float t2 = std::fmod(t * size, 1.0f);
    int idx = int(t * size);

    result = CatmullRomInterpolation(
        tmpControlPoints[std::clamp(idx - 1, 0, size)],
        tmpControlPoints[idx],
        tmpControlPoints[std::clamp(idx + 1, 0, size)],
        tmpControlPoints[std::clamp(idx + 2, 0, size)],
        t2
    );

    return result;
}

// 自由な数の制御点からCatmull-Rom補間を行い、tの地点を返す関数
Vector3 MyMath::CatmullRomPosition(const std::vector<Vector3*>& controlPoints, float t){

    t = std::clamp(t, 0.0f, 1.0f);// tを0~1に収める

    Vector3 result;
    std::vector<Vector3> tmpControlPoints;

    for(int i = 0; i < controlPoints.size(); i++){
        tmpControlPoints.push_back(*controlPoints[i]);
    }

    // 要素数が必要数に達するまでコピーして追加
    if(tmpControlPoints.size() < 4){
        // 要素数が必要数に達するまでコピーして追加
        while(tmpControlPoints.size() < 4){
            tmpControlPoints.push_back(tmpControlPoints.back());
        }

        // 等間隔に制御点を修正
        ToConstantControlPoints(&tmpControlPoints);
    }

    int size = int(tmpControlPoints.size() - 1);
    float t2 = std::fmod(t * size, 1.0f);
    int idx = int(t * size);

    result = CatmullRomInterpolation(
        tmpControlPoints[std::clamp(idx - 1, 0, size)],
        tmpControlPoints[idx],
        tmpControlPoints[std::clamp(idx + 1, 0, size)],
        tmpControlPoints[std::clamp(idx + 2, 0, size)],
        t2
    );

    return result;
}

// 制御点を等間隔に修正する関数
void MyMath::ToConstantControlPoints(std::vector<Vector3>* pControlPoints){

    // 全区間の大体の長さを求める
    int kLoop = (int)pControlPoints->size() * 16;
    float totalLength = 0.0f;
    Vector3 prev = pControlPoints->front();

    for(int i = 1; i <= kLoop; i++){
        Vector3 next = CatmullRomPosition(*pControlPoints, (float)i / (float)kLoop);
        totalLength += Length(next - prev);
        prev = next;
    }

    // 1区間の長さを求める
    float interval = totalLength / (float)(pControlPoints->size() - 1);// 1区間の長さ

    // 制御点を等間隔に修正
    std::vector<Vector3> original = *pControlPoints;
    prev = original.front();
    totalLength = 0.0f;
    int prevIdx = 0;

    for(int i = 0; i < kLoop; i++){
        Vector3 next = CatmullRomPosition(original, (float)i / (float)kLoop);
        totalLength += Length(next - prev);
        prev = next;

        // 区間が一つ進んだら制御点を修正
        int idx = int(totalLength / interval);
        if(prevIdx < idx){
            (*pControlPoints)[idx] = next;
            prevIdx = idx;
        }
    }

    if(pControlPoints->size() != original.size()){ assert(false); }
}



Vector3 MyMath::TransformNormal(const Vector3& normal, const Matrix4x4& matrix){
    Vector3 result = { 0,0,0 };

    // 法線ベクトルは平行移動の影響を受けないため、上3×3の行列を使って変換する
    result.x = normal.x * matrix.m[0][0] + normal.y * matrix.m[1][0] + normal.z * matrix.m[2][0];
    result.y = normal.x * matrix.m[0][1] + normal.y * matrix.m[1][1] + normal.z * matrix.m[2][1];
    result.z = normal.x * matrix.m[0][2] + normal.y * matrix.m[1][2] + normal.z * matrix.m[2][2];

    return result;
}


//================================================================
//                      型変換の関数
//================================================================


// floatをuint32_tに変換する関数
uint32_t MyMath::FloatToUint32(float value){
    uint32_t result;
    std::memcpy(&result, &value, sizeof(value));
    return result;
}

// uint32_tをfloatに変換する関数
float MyMath::Uint32ToFloat(uint32_t value){
    float result;
    std::memcpy(&result, &value, sizeof(value));
    return result;
}


//================================================================
//                        色を扱う関数
//================================================================

// 色の各成分を求める関数
uint32_t MyMath::Red(uint32_t color){ return (color >> 24) & 0xFF; }
uint32_t MyMath::Green(uint32_t color){ return (color >> 16) & 0xFF; }
uint32_t MyMath::Blue(uint32_t color){ return (color >> 8) & 0xFF; }
uint32_t MyMath::Alpha(uint32_t color){ return color & 0xFF; }

// RGBA形式のカラーコードをグレースケールに変換する関数
uint32_t MyMath::GrayScale(uint32_t color){
    /*
            真っ白のとき(RGB最大値のとき)が最大値の"1"だとして
          そのときRGBはそれぞれどの程度白く見えますかというのが重み

                R: 0.2126    G: 0.7152   B: 0.0722

            RGBが合わさると白になるので、上の合計値は1になる。
            グレースケールはRGBがすべて同じ値になる必要がある。
              明度のイメージを損なわないように変換するには

               "今の色 * 重み" で各色の補正後の色を求め
             すべての要素を足すことで0~1で平均明度を得られる
        カラーコードの各色を平均明度にすることでグレースケールが求まる
    */
    float trancedRed = float(Red(color)) * 0.2126f;
    float trancedGreen = float(Green(color)) * 0.7152f;
    float trancedBlue = float(Blue(color)) * 0.0722f;

    int gray = int(trancedRed + trancedGreen + trancedBlue);

    return 0xFF + (gray << 24) + (gray << 16) + (gray << 8);
}


// RGBA形式のカラーコードをVector4形式に変換する関数 (各要素は0~1に収まる)
Vector4 MyMath::FloatColor(uint32_t color){
    float delta = 1.0f / 255.0f;

    Vector4 colorf = {
        float(Red(color)) * delta,
        float(Green(color)) * delta,
        float(Blue(color)) * delta,
        float(Alpha(color)) * delta
    };

    return colorf;
}

Vector4 MyMath::FloatColor(uint32_t r, uint32_t g, uint32_t b, uint32_t a){
    float delta = 1.0f / 255.0f;

    Vector4 colorf = {
        float(r) * delta,
        float(g) * delta,
        float(b) * delta,
        float(a) * delta
    };

    return colorf;
}

uint32_t MyMath::IntColor(const Vector4& color){
    uint32_t red = std::clamp(int(color.x * 255.0f), 0, 255) << 24;
    uint32_t green = std::clamp(int(color.y * 255.0f), 0, 255) << 16;
    uint32_t blue = std::clamp(int(color.z * 255.0f), 0, 255) << 8;
    uint32_t alpha = std::clamp(int(color.w * 255.0f), 0, 255);

    return red + green + blue + alpha;
}

Vector4 MyMath::HSV_to_RGB(float h, float s, float v, float alpha){

    // 彩度が0なので明度のみを反映
    if(s == 0.0){
        return Vector4(v, v, v, alpha);
    }

    h *= 6.0;
    int i = int(h);
    float f = h - i;

    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    if(i % 6 == 0){
        return  Vector4(v, t, p, alpha);
    } else if(i % 6 == 1){
        return  Vector4(q, v, p, alpha);
    } else if(i % 6 == 2){
        return  Vector4(p, v, t, alpha);
    } else if(i % 6 == 3){
        return  Vector4(p, q, v, alpha);
    } else if(i % 6 == 4){
        return  Vector4(t, p, v, alpha);
    } else{
        return  Vector4(v, p, q, alpha);
    }
}


Vector4 MyMath::HSV_to_RGB(Vector4 HSVA_color){
    return HSV_to_RGB(HSVA_color.x, HSVA_color.y, HSVA_color.z, HSVA_color.w);
}
