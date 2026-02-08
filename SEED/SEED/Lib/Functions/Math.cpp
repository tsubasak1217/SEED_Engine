#include <SEED/Lib/Functions/Math.h>
#include <cmath>
#include <assert.h>
#include <numbers>

namespace SEED {
    namespace Methods{
        namespace Math{
            //================================================================
            //                      数学的な関数
            //================================================================

            /*---------------------------- 長さを計る関数 ----------------------------*/

            float Length(const Vector2& pos1, const Vector2& pos2){
                float xLength = (pos1.x - pos2.x);
                float yLength = (pos1.y - pos2.y);
                return std::sqrt(xLength * xLength + yLength * yLength);
            }

            float Length(const Vector3& pos1, const Vector3& pos2){
                float xLength = (pos1.x - pos2.x);
                float yLength = (pos1.y - pos2.y);
                float zLength = (pos1.z - pos2.z);
                return std::sqrt(xLength * xLength + yLength * yLength + zLength * zLength);
            }

            float Length(const Vector2& vec){
                return std::sqrt(vec.x * vec.x + vec.y * vec.y);
            }

            float Length(const Vector3& vec){
                return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
            }

            float LengthSq(const Vector2& pos1, const Vector2& pos2){
                float xLength = (pos1.x - pos2.x);
                float yLength = (pos1.y - pos2.y);
                return xLength * xLength + yLength * yLength;
            }

            float LengthSq(const Vector3& pos1, const Vector3& pos2){
                float xLength = (pos1.x - pos2.x);
                float yLength = (pos1.y - pos2.y);
                float zLength = (pos1.z - pos2.z);
                return xLength * xLength + yLength * yLength + zLength * zLength;
            }

            float LengthSq(const Vector2& vec){
                return vec.x * vec.x + vec.y * vec.y;
            }

            float LengthSq(const Vector3& vec){
                return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
            }

            bool HasLength(const Vector2& vec){
                return LengthSq(vec) > 0.0f;
            }

            bool HasLength(const Vector3& vec){
                return LengthSq(vec) > 0.0f;
            }

            /*----------------------- ベクトルを正規化する関数 ------------------------*/

            Vector2 Normalize(const Vector2& vec){
                float length = Length(vec);
                return length != 0 ? vec / Length(vec) : Vector2(0.0f, 0.0f);
            }

            Vector3 Normalize(const Vector3& vec){
                float length = Length(vec);
                return length != 0 ? vec / Length(vec) : Vector3(0.0f, 0.0f, 0.0f);
            }


            /*- 仮: 非線形を線形に変換する関数-*/
            float DepthToLinear(float depth, float near, float far){
                float z = depth * 2.0f - 1.0f;
                float linearDepth = (2.0f * near * far) / (far + near - z * (far - near));
                return (linearDepth - near) / (far - near);
            }

            /*-------------------------- 内積を求める関数 ---------------------------*/


            float Dot(const Vector2& a, const Vector2& b){
                return (a.x * b.x) + (a.y * b.y);
            }

            float Dot(const Vector3& a, const Vector3& b){
                return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
            }

            float Dot(const Vector2& pos1, const Vector2& pos2, const Vector2& targetPos){
                return Dot(pos2 - pos1, targetPos - pos1);
            }

            float Dot(const Vector3& pos1, const Vector3& pos2, const Vector3& targetPos){
                return Dot(pos2 - pos1, targetPos - pos1);
            }


            float DotNormal(const Vector2& vec1, const Vector2& vec2){
                float length = Length(vec1);
                return length != 0.0f ? Dot(vec1, vec2) / length : 0.0f;
            }

            float DotNormal(const Vector3& vec1, const Vector3& vec2){
                float length = Length(vec1);
                return length != 0.0f ? Dot(vec1, vec2) / length : 0.0f;
            }

            float DotNormal(const Vector2& pos1, const Vector2& pos2, const Vector2& targetPos){
                Vector2 lineVector = pos2 - pos1;
                Vector2 forTarget = targetPos - pos1;
                float lineLength = Length(lineVector);

                return lineLength != 0 ? Dot(lineVector, forTarget) / lineLength : 0.0f;
            }


            float DotNormal(const Vector3& pos1, const Vector3& pos2, const Vector3& targetPos){
                Vector3 lineVector = pos2 - pos1;
                Vector3 forTarget = targetPos - pos1;
                float lineLength = Length(lineVector);

                return lineLength != 0 ? Dot(lineVector, forTarget) / lineLength : 0.0f;
            }

            /*-------------------------- 外積を求める関数 ---------------------------*/

            float Cross(const Vector2& vec1, const Vector2& vec2){
                return vec1.x * vec2.y - vec1.y * vec2.x;
            }

            Vector3 Cross(const Vector3& vec1, const Vector3& vec2, bool kViewMode){
                if(kViewMode == Methods::Math::kScreen){
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

            float Cross(const Vector2& originPos, const Vector2& endPos, const Vector2& targetPos){
                float length = Length(endPos - originPos);
                return length != 0 ? Cross(endPos - originPos, targetPos - originPos) / length : 0.0f;
            }

            /*---------------------- 射影ベクトルを求める関数 -----------------------*/

            Vector2 ProjectVec(const Vector2& pos1, const Vector2& pos2, const Vector2& targetPos){
                Vector2 vec = pos2 - pos1;
                return vec * DotNormal(pos1, pos2, targetPos);
            }

            Vector3 ProjectVec(const Vector3& pos1, const Vector3& pos2, const Vector3& targetPos){
                Vector3 vec = pos2 - pos1;
                return vec * DotNormal(pos1, pos2, targetPos);
            }

            Vector2 ProjectVec(const Vector2& vec1, const Vector2& vec2){
                return vec1 * DotNormal(vec1, vec2);
            }

            Vector3 ProjectVec(const Vector3& vec1, const Vector3& vec2){
                return vec1 * DotNormal(vec1, vec2);
            }

            /*---------------------- 直線に対する最近傍点を求める関数 -----------------------*/

            Vector2 ClosestPoint(const Vector2& seg_origin, const Vector2& seg_end, const Vector2& point){
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

            Vector3 ClosestPoint(const Vector3& seg_origin, const Vector3& seg_end, const Vector3& point){
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

            std::array<Vector2, 2> LineClosestPoints(const Topology::Line2D& l1, const Topology::Line2D& l2){
                // 2直線の方向ベクトル
                Vector2 dir1 = l1.end_ - l1.origin_;
                Vector2 dir2 = l2.end_ - l2.origin_;

                // 2直線の長さ
                float length1 = Length(dir1);
                float length2 = Length(dir2);

                // 点が同じ場所の場合
                if(length1 == 0.0f){
                    if(length2 == 0.0f){
                        return { l1.origin_,l2.origin_ };
                    } else{
                        return { l1.origin_,ClosestPoint(l2.origin_,l2.end_,l1.origin_) };
                    }
                } else if(length2 == 0.0f){
                    if(length1 == 0.0f){
                        return { l1.origin_,l2.origin_ };
                    } else{
                        return { ClosestPoint(l1.origin_,l1.end_,l2.origin_),l2.origin_ };
                    }
                }

                // 2直線の始点間のベクトル
                Vector2 originVec = l2.origin_ - l1.origin_;

                // 2直線の方向ベクトルの外積
                float cross = Cross(dir1, dir2);

                // 2直線が平行な場合
                if(std::abs(cross) < 1e-6f){
                    return { l1.origin_,l2.origin_ };
                }

                // 最近傍点を計算
                float crossLengthSq = cross * cross;
                float t1 = Dot(Cross(originVec, dir2), Vector3(0.0f, 0.0f, cross)) / crossLengthSq;
                float t2 = Dot(Cross(originVec, dir1), Vector3(0.0f, 0.0f, cross)) / crossLengthSq;

                // t1, t2 を [0, 1] に制限
                t1 = std::clamp(t1, 0.0f, 1.0f);
                t2 = std::clamp(t2, 0.0f, 1.0f);

                Vector2 closest1 = l1.origin_ + dir1 * t1;
                Vector2 closest2 = l2.origin_ + dir2 * t2;
                return { closest1, closest2 };
            }

            float GetTheta(const Vector2& dir){
                return std::atan2(dir.y, dir.x); // -π ~ +π の範囲
            }


            //================================================================
            //                      個人用な便利関数
            //================================================================

            // 負数を0にする関数
            uint32_t negaZero(int32_t num){ return num > 0 ? num : 0; }
            float negaZero(float num){ return num > 0.0f ? num : 0.0f; }

            // 度をラジアンに変換する関数
            float Deg2Rad(float deg){
                return deg * (static_cast<float>(std::numbers::pi) / 180.0f);
            }

            // tを配列の要素数に変換する関数
            int32_t CalcElement(float t, int32_t size){
                int idx = int(t * (size - 1));
                return idx;
            }

            // tを現在の区間のtに変換する関数
            float CalcSectionT(float t, int32_t size){
                float t2 = std::fmod(t * (size - 1), 1.0f);
                return t2;
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            // 指定範囲を繰り返す関数 (最大値を超えたら最小値へ戻る)
            //////////////////////////////////////////////////////////////////////////////////////////////////////////

            int32_t Spiral(int32_t input, int32_t min, int32_t max){

                if(max < min){
                    assert(false);
                }

                if(min == max){
                    return min; // minとmaxが同じ場合はその値を返す
                }

                int32_t range = max - min + 1; // 🔧 範囲のサイズ
                int32_t offset = input - min;

                // 正の mod を保つようにする（負数対応）
                int32_t wrapped = ((offset % range) + range) % range;

                return min + wrapped;

            }


            float Spiral(float input, float min, float max){

                if(max < min){
                    return Spiral(input, max, min);
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

            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            // 範囲に含まれているかを判定する関数
            //////////////////////////////////////////////////////////////////////////////////////////////////////////

            bool IsContain(const Range1D& range, float value){
                return range.min <= value && value <= range.max;
            }

            bool IsContain(const Range2D& range, const Vector2& value){
                Range1D xRange = { range.min.x,range.max.x };
                Range1D yRange = { range.min.y,range.max.y };
                return IsContain(xRange, value.x) && IsContain(yRange, value.y);
            }

            bool IsContain(const Range3D& range, const Vector3& value){
                Range1D xRange = { range.min.x,range.max.x };
                Range1D yRange = { range.min.y,range.max.y };
                Range1D zRange = { range.min.z,range.max.z };
                return IsContain(xRange, value.x) && IsContain(yRange, value.y) && IsContain(zRange, value.z);
            }

            //================================================================
            //                      角度補間の関数
            //================================================================
            float LerpShortAngle(float a, float b, float t){
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

            //================================================================
            //                      線形補完
            //================================================================
            float Lerp(const float v1, const float v2, float t){
                return v1 + (v2 - v1) * t;
            }

            Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t){
                return v1 + (v2 - v1) * t;
            }

            Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t){
                return v1 + (v2 - v1) * t;
            }

            Vector4 Lerp(const Vector4& v1, const Vector4& v2, float t){
                return v1 + (v2 - v1) * t;
            }

            //================================================================
            //                      ベジェ曲線の関数
            //================================================================
            Vector3 Bezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, float t){
                return Lerp(Lerp(p0, p1, t), Lerp(p1, p2, t), t);
            }

            Vector2 Bezier(const Vector2& p0, const Vector2& p1, const Vector2& p2, float t){
                return Lerp(Lerp(p0, p1, t), Lerp(p1, p2, t), t);
            }

            Vector2 Bezier(const std::vector<Vector2>& points, float t){
                if(points.size() == 0){ return{ 0.0f,0.0f }; }
                if(points.size() == 1){ return points[0]; }
                if(points.size() == 2){ return Lerp(points[0], points[1], t); }

                // De Casteljauのアルゴリズムを使用してベジェ曲線を計算
                std::vector<Vector2> tmpPoints = points;
                while(tmpPoints.size() > 1){
                    std::vector<Vector2> nextPoints;
                    for(size_t i = 0; i < tmpPoints.size() - 1; i++){
                        nextPoints.push_back(Lerp(tmpPoints[i], tmpPoints[i + 1], t));
                    }
                    tmpPoints = nextPoints;
                }
                return tmpPoints[0];
            }

            //================================================================
            //                      スプライン補間の関数
            //================================================================

            // 4点を直接指定してCatmull-Rom補間を行う関数
            Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t){

                t = std::clamp(t, 0.0f, 1.0f);// tを0~1に収める
                if(t <= 0.0f){
                    return p1;
                } else if(t >= 1.0f){
                    return p2;
                }
                float t2 = t * t;
                float t3 = t2 * t;

                Vector3 e3 = (p3 * 1) + (p2 * -3) + (p1 * 3) + (p0 * -1);
                Vector3 e2 = (p3 * -1) + (p2 * 4) + (p1 * -5) + (p0 * 2);
                Vector3 e1 = (p2 * 1) + (p0 * -1);
                Vector3 e0 = p1 * 2;

                return ((e3 * t3) + (e2 * t2) + (e1 * t) + e0) * 0.5f;
            }

            // 4点を直接指定してCatmull-Rom補間を行う関数 (2次元版)
            Vector2 CatmullRomInterpolation(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3, float t){
                t = std::clamp(t, 0.0f, 1.0f);// tを0~1に収める
                if(t <= 0.0f){
                    return p1;
                } else if(t >= 1.0f){
                    return p2;
                }
                float t2 = t * t;
                float t3 = t2 * t;

                Vector2 e3 = (p3 * 1) + (p2 * -3) + (p1 * 3) + (p0 * -1);
                Vector2 e2 = (p3 * -1) + (p2 * 4) + (p1 * -5) + (p0 * 2);
                Vector2 e1 = (p2 * 1) + (p0 * -1);
                Vector2 e0 = p1 * 2;

                return ((e3 * t3) + (e2 * t2) + (e1 * t) + e0) * 0.5f;
            }

            // 4点を直接指定してCatmull-Rom補間を行う関数 (float版)
            float CatmullRomInterpolation(const float p0, const float p1, const float p2, const float p3, float t){
                t = std::clamp(t, 0.0f, 1.0f);// tを0~1に収める
                if(t <= 0.0f){
                    return p1;
                } else if(t >= 1.0f){
                    return p2;
                }
                float t2 = t * t;
                float t3 = t2 * t;
                float e3 = (p3 * 1) + (p2 * -3) + (p1 * 3) + (p0 * -1);
                float e2 = (p3 * -1) + (p2 * 4) + (p1 * -5) + (p0 * 2);
                float e1 = (p2 * 1) + (p0 * -1);
                float e0 = p1 * 2;
                return ((e3 * t3) + (e2 * t2) + (e1 * t) + e0) * 0.5f;
            }

            // 内部で使用する関数
            int32_t CatmullRomIndex(int32_t curIdx, int32_t size, bool connectEdge){
                if(connectEdge){
                    return curIdx < 0 ? (size + curIdx) % size : curIdx % size;
                } else{
                    return std::clamp(curIdx, 0, size - 1);
                }
            }

            // 自由な数の制御点からCatmull-Rom補間を行い、tの地点を返す関数
            Vector3 MultiCatmullRom(const std::vector<Vector3>& controlPoints, float t, bool isConnectEdge){

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

                int size = int(tmpControlPoints.size());
                float t2 = std::fmod(t * (size - 1), 1.0f);
                int idx = int(t * (size - 1));

                result = CatmullRomInterpolation(
                    tmpControlPoints[CatmullRomIndex(idx - 1, size, isConnectEdge)],
                    tmpControlPoints[CatmullRomIndex(idx, size, isConnectEdge)],
                    tmpControlPoints[CatmullRomIndex(idx + 1, size, isConnectEdge)],
                    tmpControlPoints[CatmullRomIndex(idx + 2, size, isConnectEdge)],
                    t2
                );

                return result;
            }

            // 自由な数の制御点からCatmull-Rom補間を行い、tの地点を返す関数
            Vector3 MultiCatmullRom(const std::vector<Vector3*>& controlPoints, float t, bool isConnectEdge){

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

                int size = int(tmpControlPoints.size());
                float t2 = std::fmod(t * (size - 1), 1.0f);
                int idx = int(t * (size - 1));

                result = CatmullRomInterpolation(
                    tmpControlPoints[CatmullRomIndex(idx - 1, size, isConnectEdge)],
                    tmpControlPoints[CatmullRomIndex(idx, size, isConnectEdge)],
                    tmpControlPoints[CatmullRomIndex(idx + 1, size, isConnectEdge)],
                    tmpControlPoints[CatmullRomIndex(idx + 2, size, isConnectEdge)],
                    t2
                );

                return result;
            }

            // 自由な数の制御点からCatmull-Rom補間を行い、tの地点を返す関数 (2次元版)
            Vector2 MultiCatmullRom(const std::vector<Vector2>& controlPoints, float t, bool isConnectEdge){
                if(controlPoints.size() == 0){ return{ 0.0f,0.0f }; }
                t = std::clamp(t, 0.0f, 1.0f);// tを0~1に収める
                Vector2 result;
                std::vector<Vector2> tmpControlPoints = controlPoints;
                // 要素数が必要数に達するまでコピーして追加
                while(tmpControlPoints.size() < 4){
                    // 要素数が必要数に達するまでコピーして追加
                    while(tmpControlPoints.size() < 4){
                        tmpControlPoints.push_back(tmpControlPoints.back());
                    }
                    // 等間隔に制御点を修正
                    //ToConstantControlPoints(&tmpControlPoints);
                }
                int size = int(tmpControlPoints.size());
                float t2 = std::fmod(t * (size - 1), 1.0f);
                int idx = int(t * (size - 1));

                result = CatmullRomInterpolation(
                    tmpControlPoints[CatmullRomIndex(idx - 1, size, isConnectEdge)],
                    tmpControlPoints[CatmullRomIndex(idx, size, isConnectEdge)],
                    tmpControlPoints[CatmullRomIndex(idx + 1, size, isConnectEdge)],
                    tmpControlPoints[CatmullRomIndex(idx + 2, size, isConnectEdge)],
                    t2
                );
                return result;
            }

            // 自由な数の制御点からCatmull-Rom補間を行い、tの地点を返す関数 (float版)
            float MultiCatmullRom(const std::vector<float>& controlPoints, float t, bool isConnectEdge){
                if(controlPoints.size() == 0){ return 0.0f; }
                t = std::clamp(t, 0.0f, 1.0f);// tを0~1に収める
                float result = 0.0f;
                std::vector<float> tmpControlPoints = controlPoints;
                // 要素数が必要数に達するまでコピーして追加
                while(tmpControlPoints.size() < 4){
                    // 要素数が必要数に達するまでコピーして追加
                    while(tmpControlPoints.size() < 4){
                        tmpControlPoints.push_back(tmpControlPoints.back());
                    }
                }
                int size = int(tmpControlPoints.size());
                float t2 = std::fmod(t * (size - 1), 1.0f);
                int idx = int(t * (size - 1));

                result = CatmullRomInterpolation(
                    tmpControlPoints[CatmullRomIndex(idx - 1, size, isConnectEdge)],
                    tmpControlPoints[CatmullRomIndex(idx, size, isConnectEdge)],
                    tmpControlPoints[CatmullRomIndex(idx + 1, size, isConnectEdge)],
                    tmpControlPoints[CatmullRomIndex(idx + 2, size, isConnectEdge)],
                    t2
                );
                return result;
            }

            // 制御点を等間隔に修正する関数
            void ToConstantControlPoints(std::vector<Vector3>* pControlPoints){

                // 全区間の大体の長さを求める
                int kLoop = (int)pControlPoints->size() * 16;
                float totalLength = 0.0f;
                Vector3 prev = pControlPoints->front();

                for(int i = 1; i <= kLoop; i++){
                    Vector3 next = MultiCatmullRom(*pControlPoints, (float)i / (float)kLoop);
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
                    Vector3 next = MultiCatmullRom(original, (float)i / (float)kLoop);
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


            // 制御点を等間隔に修正する関数 (2次元版)
            void ToConstantControlPoints(std::vector<Vector2>* pControlPoints){
                // 全区間の大体の長さを求める
                int kLoop = (int)pControlPoints->size() * 16;
                float totalLength = 0.0f;
                Vector2 prev = pControlPoints->front();
                for(int i = 1; i <= kLoop; i++){
                    Vector2 next = MultiCatmullRom(*pControlPoints, (float)i / (float)kLoop);
                    totalLength += Length(next - prev);
                    prev = next;
                }
                // 1区間の長さを求める
                float interval = totalLength / (float)(pControlPoints->size() - 1);// 1区間の長さ
                // 制御点を等間隔に修正
                std::vector<Vector2> original = *pControlPoints;
                prev = original.front();
                totalLength = 0.0f;
                int prevIdx = 0;
                for(int i = 0; i < kLoop; i++){
                    Vector2 next = MultiCatmullRom(original, (float)i / (float)kLoop);
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


            Vector3 TransformNormal(const Vector3& normal, const Matrix4x4& matrix){
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
            uint32_t FloatToUint32(float value){
                uint32_t result;
                std::memcpy(&result, &value, sizeof(value));
                return result;
            }

            // uint32_tをfloatに変換する関数
            float Uint32ToFloat(uint32_t value){
                float result;
                std::memcpy(&result, &value, sizeof(value));
                return result;
            }


            //================================================================
            //                        色を扱う関数
            //================================================================

            // 色の各成分を求める関数
            uint32_t Red(uint32_t color){ return (color >> 24) & 0xFF; }
            uint32_t Green(uint32_t color){ return (color >> 16) & 0xFF; }
            uint32_t Blue(uint32_t color){ return (color >> 8) & 0xFF; }
            uint32_t Alpha(uint32_t color){ return color & 0xFF; }

            // RGBA形式のカラーコードをグレースケールに変換する関数
            uint32_t GrayScale(uint32_t color){
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
            Vector4 FloatColor(uint32_t color, bool isCorrectionToLiner){
                float delta = 1.0f / 255.0f;

                Vector4 colorf = {
                    float(Red(color)) * delta,
                    float(Green(color)) * delta,
                    float(Blue(color)) * delta,
                    float(Alpha(color)) * delta
                };

                if(isCorrectionToLiner){
                    // ガンマ補正を行う
                    colorf.x = std::pow(colorf.x, 2.2f);
                    colorf.y = std::pow(colorf.y, 2.2f);
                    colorf.z = std::pow(colorf.z, 2.2f);
                }

                return colorf;
            }

            Vector4 FloatColor(uint32_t r, uint32_t g, uint32_t b, uint32_t a, bool isCorrectionToLiner){
                float delta = 1.0f / 255.0f;

                Vector4 colorf = {
                    float(r) * delta,
                    float(g) * delta,
                    float(b) * delta,
                    float(a) * delta
                };


                if(isCorrectionToLiner){
                    // ガンマ補正を行う
                    colorf.x = std::pow(colorf.x, 2.2f);
                    colorf.y = std::pow(colorf.y, 2.2f);
                    colorf.z = std::pow(colorf.z, 2.2f);
                }

                return colorf;
            }

            uint32_t IntColor(const Vector4& color){
                uint32_t red = std::clamp(int(color.x * 255.0f), 0, 255) << 24;
                uint32_t green = std::clamp(int(color.y * 255.0f), 0, 255) << 16;
                uint32_t blue = std::clamp(int(color.z * 255.0f), 0, 255) << 8;
                uint32_t alpha = std::clamp(int(color.w * 255.0f), 0, 255);

                return red + green + blue + alpha;
            }

            uint32_t IntColor(uint32_t r, uint32_t g, uint32_t b, uint32_t a, bool isCorrectionToLiner){

                // 各要素を0~1floatに変換
                Vector4 colorf = {
                    float(r) / 255.0f,
                    float(g) / 255.0f,
                    float(b) / 255.0f,
                    float(a) / 255.0f
                };

                // ガンマ補正を行う
                if(isCorrectionToLiner){
                    colorf.x = std::pow(colorf.x, 2.2f);
                    colorf.y = std::pow(colorf.y, 2.2f);
                    colorf.z = std::pow(colorf.z, 2.2f);
                }

                // 各要素を0~255に変換してからuint32_tに変換
                uint32_t red = std::clamp(int(colorf.x * 255.0f), 0, 255) << 24;
                uint32_t green = std::clamp(int(colorf.y * 255.0f), 0, 255) << 16;
                uint32_t blue = std::clamp(int(colorf.z * 255.0f), 0, 255) << 8;
                uint32_t alpha = std::clamp(int(colorf.w * 255.0f), 0, 255);

                return red + green + blue + alpha;
            }

            Vector4 HSV_to_RGB(float h, float s, float v, float alpha){

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



            Vector4 HSV_to_RGB(const Vector4& HSVA_color){
                return HSV_to_RGB(HSVA_color.x, HSVA_color.y, HSVA_color.z, HSVA_color.w);
            }

            // RGBをHSV形式に変換する関数
            Vector4 RGB_to_HSV(const Vector4& rgbColor){
                float r = rgbColor.x;
                float g = rgbColor.y;
                float b = rgbColor.z;
                float max = std::max({ r, g, b });
                float min = std::min({ r, g, b });
                float delta = max - min;
                float h, s, v = max;
                if(max == 0.0f){
                    s = 0.0f; // 彩度が0
                    h = 0.0f; // 色相は定義されない
                } else{
                    s = delta / max; // 彩度の計算
                    if(delta == 0.0f){
                        h = 0.0f; // 色相は定義されない
                    } else if(max == r){
                        h = (g - b) / delta + (g < b ? 6.0f : 0.0f);
                    } else if(max == g){
                        h = (b - r) / delta + 2.0f;
                    } else{
                        h = (r - g) / delta + 4.0f;
                    }
                    h /= 6.0f; // 色相を[0,1]に正規化
                }
                return Vector4(h, s, v, rgbColor.w);
            }

            // RGB形式のカラーコードをHSV形式に変換する関数
            Vector4 RGB_to_HSV(uint32_t colorCode){
                Vector4 rgbColor = FloatColor(colorCode);
                return RGB_to_HSV(rgbColor);
            }
        }
    }
} // namespace SEED
