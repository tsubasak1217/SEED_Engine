#include <SEED/Lib/Functions/SRTFunc.h>
#include <SEED/Lib/Functions/MatrixMath.h>
#include <SEED/Lib/Functions/Math.h>
#include <cmath>
#include <numbers>
#include <cassert>

namespace SEED{
    namespace Methods{
        namespace SRT{

            //----------------- thetaとphiからベクトルを生成する関数 -----------------//
            Vector3 CreateVector(float theta, float phi){
                // 球座標から直交座標への変換（左手座標系用）
                float x = std::sin(phi) * std::cos(theta);
                float y = std::cos(phi);
                float z = std::sin(phi) * std::sin(theta);

                return { x,y,z };
            }

            // 内部で使用する関数
            int32_t CatmullRomIndex(int32_t curIdx, int32_t size, bool connectEdge){
                if(connectEdge){
                    return curIdx < 0 ? (size + curIdx) % size : curIdx % size;
                } else{
                    return std::clamp(curIdx, 0, size - 1);
                }
            }

            //================================================================
            //                      クォータニオン補間の関数
            //================================================================

            // クォータニオンの接線を計算する関数
            Quaternion ComputeTangent(const Quaternion& qprev, const Quaternion& q, const Quaternion& qnext){
                Quaternion invQ = q.Inverse();
                Quaternion log1 = Quaternion::Log(invQ * qnext);
                Quaternion log2 = Quaternion::Log(invQ * qprev);
                Quaternion expPart = Quaternion::Exp((log1 + log2) * (-0.25f));
                return q * expPart;
            }

            // Squad補間(4点から補間を行うQuaternionの補間。Splineみたいな感じ)
            Quaternion Squad(const Quaternion& q1, const Quaternion& q2, const Quaternion& a, const Quaternion& b, float t){
                Quaternion slerp1 = Quaternion::Slerp(q1, q2, t);
                Quaternion slerp2 = Quaternion::Slerp(a, b, t);
                return Quaternion::Slerp(slerp1, slerp2, 2 * t * (1 - t));
            }

            // 複数のクォータニオンからSquad補間を行う関数
            Quaternion MultiSquad(const std::vector<Quaternion>& quats, float t, bool isConnectEdge){

                // 入力チェック
                if(quats.size() == 0){ return{ 0.0f,0.0f }; }

                // tを0~1に収める
                t = std::clamp(t, 0.0f, 1.0f);

                // 要素数が必要数に達するまでコピーして追加
                std::vector<Quaternion> tmpQuats = quats;
                while(tmpQuats.size() < 4){
                    // 要素数が必要数に達するまでコピーして追加
                    while(tmpQuats.size() < 4){
                        tmpQuats.push_back(tmpQuats.back());
                    }
                }

                // 区間とその中でのtを求める
                int size = int(tmpQuats.size());
                float localT = std::fmod(t * (size - 1), 1.0f);
                int idx = int(t * (size - 1));

                // 4点を取得して補間を行う
                const Quaternion& q0 = tmpQuats[CatmullRomIndex(idx - 1, size, isConnectEdge)];
                const Quaternion& q1 = tmpQuats[CatmullRomIndex(idx, size, isConnectEdge)];
                const Quaternion& q2 = tmpQuats[CatmullRomIndex(idx + 1, size, isConnectEdge)];
                const Quaternion& q3 = tmpQuats[CatmullRomIndex(idx + 2, size, isConnectEdge)];

                Quaternion a = ComputeTangent(q0, q1, q2);
                Quaternion b = ComputeTangent(q1, q2, q3);

                return Squad(q1, q2, a, b, localT);
            }


            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            // ベクトルから三次元の回転角を算出する関数
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            Vector3 CalcRotateVec(const Vector3& vec){
                Vector3 rotate = { 0.0f,0.0f,0.0f };

                // ベクトルの長さを計算
                float length = Math::Length(vec);
                if(length != 0.0f){
                    // 正規化されたベクトル
                    Vector3 normalizedVec = vec / length;

                    // X軸回りの回転角
                    rotate.x = -std::asin(normalizedVec.y); // Y成分で縦方向の角度を決定

                    // Y軸回りの回転角
                    rotate.y = std::atan2(normalizedVec.x, normalizedVec.z); // XとZの比率で横方向の角度を決定
                }

                return rotate;
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            // ベクトルから三次元の回転角を算出する関数
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            Vector3 CalcRotateFromVec(const Vector3& vec){
                Vector3 rotate = { 0.0f,0.0f,0.0f };

                // ベクトルの長さを計算
                float length = Math::Length(vec);
                if(length != 0.0f){
                    // 正規化されたベクトル
                    Vector3 normalizedVec = vec / length;

                    // X軸回りの回転角
                    rotate.x = -std::asin(normalizedVec.y); // Y成分で縦方向の角度を決定

                    // Y軸回りの回転角
                    rotate.y = std::atan2(normalizedVec.x, normalizedVec.z); // XとZの比率で横方向の角度を決定
                }

                return rotate;
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            // トランスフォームの補間を行う関数
            //////////////////////////////////////////////////////////////////////////////////////////////////////////

            // 3D
            Transform Interpolate(const Transform& a, const Transform& b, float t){
                Transform result;
                // 位置の補間
                result.translate = Math::Lerp(a.translate, b.translate, t);
                // 回転の補間（球面線形補間を使用）
                result.rotate = Quaternion::Slerp(a.rotate, b.rotate, t);
                // スケールの補間
                result.scale = Math::Lerp(a.scale, b.scale, t);
                return result;
            }

            // 2D
            Transform2D Interpolate(const Transform2D& a, const Transform2D& b, float t){
                Transform2D result;
                // 位置の補間
                result.translate = Math::Lerp(a.translate, b.translate, t);
                // 回転の補間
                result.rotate = Math::Lerp(a.rotate, b.rotate, t);
                // スケールの補間
                result.scale = Math::Lerp(a.scale, b.scale, t);
                return result;
            }

            // 2D 複数のトランスフォームから補間
            Transform2D Interpolate(const std::vector<Transform2D>& transforms, float t){
                int32_t size = static_cast<int32_t>(transforms.size());
                int32_t currentIdx = Math::CalcElement(t, size);
                float sectionT = Math::CalcSectionT(t, size);
                return Interpolate(transforms[currentIdx], transforms[std::clamp(currentIdx + 1, 0, size - 1)], sectionT);
            }

            // 3D 複数のトランスフォームから補間
            Transform Interpolate(const std::vector<Transform>& transforms, float t){
                int32_t size = static_cast<int32_t>(transforms.size());
                int32_t currentIdx = Math::CalcElement(t, size);
                float sectionT = Math::CalcSectionT(t, size);
                return Interpolate(transforms[currentIdx], transforms[std::clamp(currentIdx + 1, 0, size - 1)], sectionT);
            }

            // Quaternionを複数配列から補間
            Quaternion Interpolate(const std::vector<Quaternion>& transforms, float t){
                int32_t size = static_cast<int32_t>(transforms.size());
                int32_t currentIdx = Math::CalcElement(t, size);
                float sectionT = Math::CalcSectionT(t, size);
                return Quaternion::Slerp(transforms[currentIdx], transforms[std::clamp(currentIdx + 1, 0, size - 1)], sectionT);
            }

            // 2D Catmull-Rom補間
            Transform2D CatmullRomInterpolate(const std::vector<Transform2D>& transforms, float t, bool connectEdge){
                Transform2D result;

                // 各成分をそれぞれ配列に分解
                std::vector<Vector2> scales;
                std::vector<float> rotations;
                std::vector<Vector2> positions;
                for(const auto& tr : transforms){
                    scales.push_back(tr.scale);
                    rotations.push_back(tr.rotate);
                    positions.push_back(tr.translate);
                }

                // 結果の各成分を求める
                result.scale = Math::MultiCatmullRom(scales, t, connectEdge);
                result.rotate = Math::MultiCatmullRom(rotations, t, connectEdge);
                result.translate = Math::MultiCatmullRom(positions, t, connectEdge);
                return result;
            }

            // 3D Catmull-Rom補間
            Transform CatmullRomInterpolate(const std::vector<Transform>& transforms, float t, bool connectEdge){
                Transform result;

                // 各成分をそれぞれ配列に分解
                std::vector<Vector3> scales;
                std::vector<Quaternion> rotations;
                std::vector<Vector3> positions;
                for(const auto& tr : transforms){
                    scales.push_back(tr.scale);
                    rotations.push_back(tr.rotate);
                    positions.push_back(tr.translate);
                }

                // 結果の各成分を求める
                result.scale = Math::MultiCatmullRom(scales, t, connectEdge);
                result.rotate = MultiSquad(rotations, t, connectEdge);
                result.translate = Math::MultiCatmullRom(positions, t, connectEdge);
                return result;
            }
        }
    }
} // namespace SEED