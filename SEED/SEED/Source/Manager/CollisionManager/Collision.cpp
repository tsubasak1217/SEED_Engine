#include "Collision.h"
#include <vector>
#include <algorithm>
#include <SEED/Lib/Structs/Range1D.h>
#include <SEED/Lib/Shapes/Line.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Functions/MyFunc/ShapeMath.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>

// 各形状のコライダーをインクルード
#include <SEED/Source/Basic/Collision/Collider_AABB.h>
#include <SEED/Source/Basic/Collision/Collider_OBB.h>
#include <SEED/Source/Basic/Collision/Collider_Line.h>
#include <SEED/Source/Basic/Collision/Collider_Sphere.h>
#include <SEED/Source/Basic/Collision/Collider_Capsule.h>

float separator = 0.01f;

/////////////////////////////////////////////////////////////////////////
//						重複防止のためcpp内で宣言・定義
/////////////////////////////////////////////////////////////////////////
void CalcPushbackRatio(const Collider* collider1, const Collider* collider2, CollisionData* data);

bool CheckProjentionCollision(
    std::vector<Vector3> vertices1, std::vector<Vector3>vertices2,
    const Vector3& axis, CollisionData* pData = nullptr
);
float CalcProjentionDepth(
    std::vector<Vector3> vertices1, std::vector<Vector3>vertices2,
    const Vector3& axis
);
Range1D GetProjectionRange(std::vector<Vector3> vertices, const Vector3& axis);
bool CalcProjectionDepth(const ::Line& line, const Quad& plane, const Vector3& axis, CollisionData* pData = nullptr);
bool Collision_OBB_OBB(const ::OBB& obb1, const ::OBB& obb2);
bool Collision_AABB_OBB(const ::AABB& aabb, const ::OBB& obb);
bool Collision_Line_OBB(const ::Line& line, const ::OBB& obb);
CollisionData CollisionData_Line_OBB(const Line& line, const OBB& obb);
CollisionData CollisionData_Point_OBB(const Vector3& point, const OBB& obb);
bool Collision_AABB_AABB(const ::AABB& aabb1, const ::AABB& aabb2);
bool Collision_Line_AABB(const ::Line& line, const ::AABB& aabb);
CollisionData CollisionData_Line_AABB(const Line& line, const AABB& aabb);
CollisionData CollisionData_Point_AABB(const Vector3& point, const AABB& aabb);
bool Collision_Sphere_OBB(const ::Sphere& sphere, const ::OBB& obb);
CollisionData CollisionData_MoveOBB_Sphere(Collider* obbCollider, Collider* sphereCollider);
CollisionData CollisionData_OBB_MoveSphere(Collider* obbCollider, Collider* sphereCollider);
bool Collision_Sphere_AABB(const ::Sphere& sphere, const ::AABB& aabb);
bool Collision_Sphere_Line(const ::Sphere& sphere, const ::Line& line);
bool Collision_Sphere_Sphere(const ::Sphere& sphere1, const ::Sphere& sphere2);
CollisionData CollisionData_MoveSphere_MoveSphere(Collider* sphereCollider1, Collider* sphereCollider2);
bool Collision_Quad_Line(const ::Quad& rectangle, const ::Line& line);
Vector3 Collision_Quad_Line_Normal(const::Quad& rectangle, const::Line& line);
bool Collision_Triangle_Line(const Triangle& triangle, const Line& line);
CollisionData CollisionData_Line_Plane(const Line& line, const Quad& plane);

CollisionData Collision_MoveSphere_AABB(Collider* sphereCollider, Collider* aabbCollider);
CollisionData Collision_Sphere_MoveAABB(Collider* sphereCollider, Collider* aabbCollider);
CollisionData CollisionData_Capsule_Capusle(const ::Capsule& capsule1, const ::Capsule& capsule2);
CollisionData CollisionData_Capsule_Sphere(const ::Capsule& capsule, const ::Sphere& sphere);

/////////////////////////////////////////////////////////////////////////
//						名前空間内の関数の定義
/////////////////////////////////////////////////////////////////////////

namespace Collision{

    namespace OBB{
        bool OBB(const ::OBB& obb1, const ::OBB& obb2){ return Collision_OBB_OBB(obb1, obb2); }
        bool AABB(const ::OBB& obb, const ::AABB& aabb){ return Collision_AABB_OBB(aabb, obb); }
        bool Line(const ::OBB& obb, const ::Line& line){ return Collision_Line_OBB(line, obb); }
        bool Sphere(const::OBB& obb, const::Sphere& sphere){ return Collision_Sphere_OBB(sphere, obb); }
        CollisionData Sphere(Collider* obbCollider, Collider* sphereCollider){ return CollisionData_MoveOBB_Sphere(obbCollider, sphereCollider); }
    }

    namespace AABB{
        bool OBB(const ::AABB& aabb, const ::OBB& obb){ return Collision_AABB_OBB(aabb, obb); }
        bool AABB(const ::AABB& aabb1, const ::AABB& aabb2){ return Collision_AABB_AABB(aabb1, aabb2); }
        bool Line(const ::AABB& aabb, const ::Line& line){ return Collision_Line_AABB(line, aabb); }
        bool Sphere(const ::AABB& aabb, const ::Sphere& sphere){ return Collision_Sphere_AABB(sphere, aabb); }
        CollisionData Sphere(Collider* aabbCollider, Collider* sphereCollider){ return Collision_Sphere_MoveAABB(sphereCollider, aabbCollider); }
    }

    namespace Line{
        bool OBB(const ::Line& line, const ::OBB& obb){ return Collision_Line_OBB(line, obb); }
        bool AABB(const ::Line& line, const ::AABB& aabb){ return Collision_Line_AABB(line, aabb); }
        bool Sphere(const ::Line& line, const ::Sphere& sphere){ return Collision_Sphere_Line(sphere, line); }
    }

    namespace Sphere{
        bool OBB(const ::Sphere& sphere, const ::OBB& obb){ return Collision_Sphere_OBB(sphere, obb); }
        CollisionData OBB(Collider* sphereCollider, Collider* obbCollider){ return CollisionData_OBB_MoveSphere(obbCollider, sphereCollider); }
        bool AABB(const ::Sphere& sphere, const ::AABB& aabb){ return Collision_Sphere_AABB(sphere, aabb); }
        CollisionData AABB(Collider* sphereCollider, Collider* aabbCollider){ return Collision_MoveSphere_AABB(sphereCollider, aabbCollider); }
        bool Line(const ::Sphere& sphere, const ::Line& line){ return Collision_Sphere_Line(sphere, line); }
        bool Sphere(const ::Sphere& sphere1, const ::Sphere& sphere2){ return Collision_Sphere_Sphere(sphere1, sphere2); }
        CollisionData Sphere(Collider* sphere1Collider, Collider* sphere2Collider){ return CollisionData_MoveSphere_MoveSphere(sphere1Collider, sphere2Collider); }
    }

    namespace Quad{
        CollisionData Line(const::Quad& quad, const::Line& line){ return CollisionData_Line_Plane(line, quad); }
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////																					///////////
///////////									以下に定義を記述									///////////
///////////																					///////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////
//======================================= 質量比を求める関数 =======================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////

void CalcPushbackRatio(const Collider* collider1, const Collider* collider2, CollisionData* data){

    // もしどちらかが押し戻ししない設定なら終了
    if(collider1->isGhost_ or collider2->isGhost_){
        data->pushBackRatio_A = 0.0f;
        data->pushBackRatio_B = 0.0f;
        return;
    }

    // 質量比を求める
    float mass1 = collider1->mass_;
    float mass2 = collider2->mass_;
    float sumMass = mass1 + mass2;
    if(sumMass == 0.0f){ sumMass = 1.0f; }

    data->pushBackRatio_A = 1.0f - (mass1 / sumMass);
    data->pushBackRatio_B = 1.0f - (mass2 / sumMass);

    // 動かないオブジェクトがある場合
    if(collider1->isMovable_ && !collider2->isMovable_){
        data->pushBackRatio_A = 1.0f;
        data->pushBackRatio_B = 0.0f;
    } else if(!collider1->isMovable_ && collider2->isMovable_){
        data->pushBackRatio_A = 0.0f;
        data->pushBackRatio_B = 1.0f;
    } else if(!collider1->isMovable_ && !collider2->isMovable_){
        data->pushBackRatio_A = 0.0f;
        data->pushBackRatio_B = 0.0f;
    }

    return;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//======================================= 射影範囲を取得する関数 =====================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////

Range1D GetProjectionRange(std::vector<Vector3> vertices, const Vector3& axis){
    Vector3 normalAxis = MyMath::Normalize(axis);
    float min = MyMath::Dot(vertices[0], normalAxis);
    float max = min;

    for(size_t i = 1; i < vertices.size(); i++) {
        float proj = MyMath::Dot(vertices[i], normalAxis);
        if(proj < min) min = proj;
        if(proj > max) max = proj;
    }

    return { min, max };
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//=================================== 影が重なっているか判定する関数 ===================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool CheckProjentionCollision(
    std::vector<Vector3> vertices1, std::vector<Vector3> vertices2, const Vector3& axis, CollisionData* pData
){
    // 射影した範囲を取得
    Range1D range[2];// xにはmin, yにはmaxが入る
    range[0] = GetProjectionRange(vertices1, axis);
    range[1] = GetProjectionRange(vertices2, axis);

    // すべての影の中のmin,maxを計算
    Range1D allRange = { (std::min)(range[0].min,range[1].min),(std::max)(range[0].max,range[1].max) };

    // 影の長さの合計値
    float sumLength = (range[0].max - range[0].min) + (range[1].max - range[1].min);
    // すべての影で計算したmin,maxの範囲の長さ
    float subLength = allRange.max - allRange.min;
    float depth = 0.0f;

    // 影が接触していたらtrue
    if(subLength <= sumLength){

        // 衝突データを格納
        if(pData != nullptr){
            // もし衝突対象の影が完全に含まれていたら
            if(range[0].min < range[1].min && range[0].max > range[1].max){
                depth = range[1].max - range[0].max;
            } else if(range[1].min < range[0].min && range[1].max > range[0].max){
                depth = range[1].max - range[0].min;
            } else {
                // そうでない場合は重なっている部分の長さを計算
                depth = sumLength - subLength;
            }

            Vector3 normalAxis = MyMath::Normalize(axis);
            pData->collideDepth = depth + separator;
        }

        return true;
    } else {
        return false;
    }
}

float CalcProjentionDepth(std::vector<Vector3> vertices1, std::vector<Vector3> vertices2, const Vector3& axis){

    // 射影した範囲を取得
    Range1D range[2];// xにはmin, yにはmaxが入る
    range[0] = GetProjectionRange(vertices1, axis);
    range[1] = GetProjectionRange(vertices2, axis);

    // すべての影の中のmin,maxを計算
    Range1D allRange = { (std::min)(range[0].min,range[1].min),(std::max)(range[0].max,range[1].max) };

    // 影の長さの合計値
    float sumLength = (range[0].max - range[0].min) + (range[1].max - range[1].min);
    // すべての影で計算したmin,maxの範囲の長さ
    float subLength = allRange.max - allRange.min;

    // 重なっている部分の長さを計算
    float depth = -1.0f;

    // 影が接触していないとき
    if(subLength > sumLength){
        depth = subLength - sumLength;
    }

    return depth;
}


bool CalcProjectionDepth(const::Line& line, const Quad& plane, const Vector3& axis, CollisionData* pData){

    // 計算用変数
    std::vector<Vector3> vertices1 = { plane.localVertex[0],plane.localVertex[1],plane.localVertex[2],plane.localVertex[3] };
    std::vector<Vector3> vertices2 = { line.origin_,line.end_ };
    Range1D range[2];

    // 平面にLineのベクトル方向の厚みを持たせる(Lineの影に完全に含まれると正しい計算ができないため)
    Vector3 vec = MyMath::Normalize(line.end_ - line.origin_);
    for(int i = 0; i < 4; i++){
        vertices1.push_back(plane.localVertex[i] + vec * 100.0f);
    }

    // 射影した範囲を取得
    range[0] = GetProjectionRange(vertices1, axis);
    range[1] = GetProjectionRange(vertices2, axis);

    // すべての影の中のmin,maxを計算
    Range1D allRange = { (std::min)(range[0].min,range[1].min),(std::max)(range[0].max,range[1].max) };

    // 影の長さの合計値
    float sumLength = (range[0].max - range[0].min) + (range[1].max - range[1].min);
    // すべての影で計算したmin,maxの範囲の長さ
    float subLength = allRange.max - allRange.min;

    // 衝突している場合
    if(subLength <= sumLength){
        // 衝突深度が格納されているものより浅ければ格納し直す
        float depth = sumLength - subLength;
        if(pData->collideDepth > depth){
            pData->collideDepth = depth + separator;
            pData->hitNormal = axis;
        }

        return true;
    } else {// 衝突していない場合
        return false;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//======================================= OBB同士の衝突判定 =========================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////

//bool Collision_OBB_OBB(const ::OBB& obb1, const ::OBB& obb2, CollisionData* pData){
//
//    float longSegmrnt[2] = {
//    (std::max)({obb1.halfSize.x * 2,obb1.halfSize.y * 2,obb1.halfSize.z * 2}),
//    (std::max)({obb2.halfSize.x * 2,obb2.halfSize.y * 2,obb2.halfSize.z * 2})
//    };
//
//    // 長い辺の長さの合計が中心間の距離よりも短ければ衝突していない
//    if(MyMath::Length(obb1.center - obb2.center) > longSegmrnt[0] + longSegmrnt[1]){
//        return false;
//    }
//
//    // ローカル頂点
//    std::vector<std::vector<Vector3>>vertices(3, std::vector<Vector3>(8));
//    vertices[0][0] = { -obb1.halfSize.x,obb1.halfSize.y,-obb1.halfSize.z };// LT
//    vertices[0][1] = { obb1.halfSize.x,obb1.halfSize.y,-obb1.halfSize.z };// RT
//    vertices[0][2] = { -obb1.halfSize.x,-obb1.halfSize.y,-obb1.halfSize.z };// LB
//    vertices[0][3] = { obb1.halfSize.x,-obb1.halfSize.y,-obb1.halfSize.z };// RB
//    vertices[0][4] = { -obb1.halfSize.x,obb1.halfSize.y,obb1.halfSize.z };// LT
//    vertices[0][5] = { obb1.halfSize.x,obb1.halfSize.y,obb1.halfSize.z };// RT
//    vertices[0][6] = { -obb1.halfSize.x,-obb1.halfSize.y,obb1.halfSize.z };// LB
//    vertices[0][7] = { obb1.halfSize.x,-obb1.halfSize.y,obb1.halfSize.z };// RB
//
//    vertices[1][0] = { -obb2.halfSize.x,obb2.halfSize.y,-obb2.halfSize.z };// LT
//    vertices[1][1] = { obb2.halfSize.x,obb2.halfSize.y,-obb2.halfSize.z };// RT
//    vertices[1][2] = { -obb2.halfSize.x,-obb2.halfSize.y,-obb2.halfSize.z };// LB
//    vertices[1][3] = { obb2.halfSize.x,-obb2.halfSize.y,-obb2.halfSize.z };// RB
//    vertices[1][4] = { -obb2.halfSize.x,obb2.halfSize.y,obb2.halfSize.z };// LT
//    vertices[1][5] = { obb2.halfSize.x,obb2.halfSize.y,obb2.halfSize.z };// RT
//    vertices[1][6] = { -obb2.halfSize.x,-obb2.halfSize.y,obb2.halfSize.z };// LB
//    vertices[1][7] = { obb2.halfSize.x,-obb2.halfSize.y,obb2.halfSize.z };// RB
//
//    vertices[2][0] = { -obb1.halfSize.x,obb1.halfSize.y,-obb1.halfSize.z };// LT
//    vertices[2][1] = { obb1.halfSize.x,obb1.halfSize.y,-obb1.halfSize.z };// RT
//    vertices[2][2] = { -obb1.halfSize.x,-obb1.halfSize.y,-obb1.halfSize.z };// LB
//    vertices[2][3] = { obb1.halfSize.x,-obb1.halfSize.y,-obb1.halfSize.z };// RB
//    vertices[2][4] = { -obb1.halfSize.x,obb1.halfSize.y,obb1.halfSize.z };// LT
//    vertices[2][5] = { obb1.halfSize.x,obb1.halfSize.y,obb1.halfSize.z };// RT
//    vertices[2][6] = { -obb1.halfSize.x,-obb1.halfSize.y,obb1.halfSize.z };// LB
//    vertices[2][7] = { obb1.halfSize.x,-obb1.halfSize.y,obb1.halfSize.z };// RB
//
//    // ワールド行列を作成
//    Matrix4x4 OBBworldMat[3] = {
//        AffineMatrix({1.0f,1.0f,1.0f},obb1.rotate,obb1.center),
//        AffineMatrix({1.0f,1.0f,1.0f},obb2.rotate,obb2.center),
//    };
//
//    // ワールド座標に変換
//    for(int32_t i = 0; i < 2; i++){
//        for(int32_t j = 0; j < 8; j++){
//            vertices[i][j] *= OBBworldMat[i];
//        }
//    }
//
//    Quad rect[6] = {
//        { vertices[1][0],vertices[1][1],vertices[1][2],vertices[1][3] },// 手前
//        { vertices[1][5],vertices[1][4],vertices[1][7],vertices[1][6] },// 奥
//        { vertices[1][1],vertices[1][0],vertices[1][5],vertices[1][4] },// 上
//        { vertices[1][2],vertices[1][3],vertices[1][6],vertices[1][7] },// 下
//        { vertices[1][0],vertices[1][2],vertices[1][4],vertices[1][6] },// 左
//        { vertices[1][3],vertices[1][1],vertices[1][7],vertices[1][5] }// 右
//    };
//
//    //ここで線分と面で当たり判定
//    Vector3 hitNormal = { 0.0f,0.0f,0.0f };
//    std::vector<float> depths;
//    std::vector<Vector3> normals;
//
//    for(int i = 0; i < 8; i++){
//        Line line = { vertices[0][i] - vertices[2][i],vertices[0][i] };
//        line.type_ = SEGMENT;
//
//        for(int j = 0; j < 6; j++){
//            Vector3 tmpNormal = Collision_Quad_Line_Normal(rect[j], line);
//            if(MyMath::Dot(pData->moveVec1, tmpNormal) < 0.0f){
//                hitNormal = tmpNormal;
//                pData->pushBackAxis = hitNormal;
//                CalcProjectionDepth(line, rect[j], hitNormal, pData);
//
//                depths.push_back(pData->collideDepth);
//                normals.push_back(hitNormal);
//            }
//        }
//    }
//
//
//    // 最も深いめり込みを取得
//    if(depths.size() > 0){
//        int minIndex = 0;
//        for(int i = 1; i < depths.size(); i++){
//            if(depths[minIndex] > depths[i]){
//                minIndex = i;
//            }
//        }
//
//        pData->collideDepth = depths[minIndex];
//        pData->pushBackAxis = normals[minIndex];
//    }
//
//
//    // 法線情報が格納されていたら当たっている
//
//    if(depths.size() != 0){
//        if(pData){
//            pData->isCollided = true;
//        }
//
//        return true;
//
//    } else{
//
//        if(pData){
//
//            std::vector<float> distances;
//
//            // 面法線
//            Vector3 normal[2][3] = {
//            {
//            vertices[0][1] - vertices[0][0],
//            vertices[0][2] - vertices[0][0],
//            vertices[0][4] - vertices[0][0]
//            },
//            {
//            vertices[1][1] - vertices[1][0],
//            vertices[1][2] - vertices[1][0],
//            vertices[1][4] - vertices[1][0]
//            }
//            };
//
//            // 面法線の中から分離軸を探す
//            for(int32_t i = 0; i < 2; i++){
//                for(int32_t j = 0; j < 3; j++){
//
//                    // 影同士の距離を調べる
//                    float distance = CalcProjentionDepth(vertices[0], vertices[1], normal[i][j]);
//                    if(distance > 0.0f){
//                        distances.push_back(distance);
//                    }
//
//                    distance = CalcProjentionDepth(vertices[0], vertices[1], -normal[i][j]);
//                    if(distance > 0.0f){
//                        distances.push_back(distance);
//                    }
//                }
//            }
//
//            // クロス積を計算
//            for(int32_t i = 0; i < 3; i++){
//                for(int32_t j = 0; j < 3; j++){
//
//                    Vector3 axis = MyMath::Cross(normal[0][i], normal[1][j]);
//                    if(MyMath::Length(axis) < 1e-6f) { // ゼロベクトルを無視
//                        continue;
//                    }
//
//                    // 影同士の距離を調べる
//                    float distance = CalcProjentionDepth(vertices[0], vertices[1], axis);
//                    if(distance > 0.0f){
//                        distances.push_back(distance);
//                    }
//
//                    distance = CalcProjentionDepth(vertices[0], vertices[1], -axis);
//                    if(distance > 0.0f){
//                        distances.push_back(distance);
//                    }
//                }
//            }
//
//
//            // 最も近いdistanceを取得
//            float max = 10000000.0f;
//            for(int i = 0; i < distances.size(); i++){
//                if(distances[i] < max){
//                    max = distances[i];
//                }
//            }
//
//            if(pData->isCollided == false){
//                if(pData->collideDepth < max){
//                    pData->collideDepth = max;
//                }
//            }
//        }
//
//        return false;
//    }
//}

bool Collision_OBB_OBB(const ::OBB& obb1, const ::OBB& obb2){

    float longSegmrnt[2] = {
    (std::max)({obb1.halfSize.x * 2,obb1.halfSize.y * 2,obb1.halfSize.z * 2}),
    (std::max)({obb2.halfSize.x * 2,obb2.halfSize.y * 2,obb2.halfSize.z * 2})
    };

    // 長い辺の長さの合計が中心間の距離よりも短ければ衝突していない
    if(MyMath::Length(obb1.center - obb2.center) > longSegmrnt[0] + longSegmrnt[1]){
        return false;
    }

    // ローカル頂点
    std::vector<std::vector<Vector3>>vertices(3, std::vector<Vector3>(8));
    vertices[0][0] = { -obb1.halfSize.x,obb1.halfSize.y,-obb1.halfSize.z };// LT
    vertices[0][1] = { obb1.halfSize.x,obb1.halfSize.y,-obb1.halfSize.z };// RT
    vertices[0][2] = { -obb1.halfSize.x,-obb1.halfSize.y,-obb1.halfSize.z };// LB
    vertices[0][3] = { obb1.halfSize.x,-obb1.halfSize.y,-obb1.halfSize.z };// RB
    vertices[0][4] = { -obb1.halfSize.x,obb1.halfSize.y,obb1.halfSize.z };// LT
    vertices[0][5] = { obb1.halfSize.x,obb1.halfSize.y,obb1.halfSize.z };// RT
    vertices[0][6] = { -obb1.halfSize.x,-obb1.halfSize.y,obb1.halfSize.z };// LB
    vertices[0][7] = { obb1.halfSize.x,-obb1.halfSize.y,obb1.halfSize.z };// RB

    vertices[1][0] = { -obb2.halfSize.x,obb2.halfSize.y,-obb2.halfSize.z };// LT
    vertices[1][1] = { obb2.halfSize.x,obb2.halfSize.y,-obb2.halfSize.z };// RT
    vertices[1][2] = { -obb2.halfSize.x,-obb2.halfSize.y,-obb2.halfSize.z };// LB
    vertices[1][3] = { obb2.halfSize.x,-obb2.halfSize.y,-obb2.halfSize.z };// RB
    vertices[1][4] = { -obb2.halfSize.x,obb2.halfSize.y,obb2.halfSize.z };// LT
    vertices[1][5] = { obb2.halfSize.x,obb2.halfSize.y,obb2.halfSize.z };// RT
    vertices[1][6] = { -obb2.halfSize.x,-obb2.halfSize.y,obb2.halfSize.z };// LB
    vertices[1][7] = { obb2.halfSize.x,-obb2.halfSize.y,obb2.halfSize.z };// RB

    // ワールド行列を作成
    Matrix4x4 OBBworldMat[2] = {
        AffineMatrix({1.0f,1.0f,1.0f},obb1.rotate,obb1.center),
        AffineMatrix({1.0f,1.0f,1.0f},obb2.rotate,obb2.center)
    };

    // ワールド座標に変換
    for(int32_t i = 0; i < 2; i++){
        for(int32_t j = 0; j < 8; j++){
            vertices[i][j] *= OBBworldMat[i];
        }
    }

    // 分離軸候補を洗い出す(法線編)
    std::vector<Vector3> axes[2];
    axes[0].push_back(vertices[0][1] - vertices[0][0]);
    axes[0].push_back(vertices[0][2] - vertices[0][0]);
    axes[0].push_back(vertices[0][4] - vertices[0][0]);
    axes[1].push_back(vertices[1][1] - vertices[1][0]);
    axes[1].push_back(vertices[1][2] - vertices[1][0]);
    axes[1].push_back(vertices[1][4] - vertices[1][0]);

    // 影の当たり判定を行う
    for(int32_t i = 0; i < 2; i++){
        for(int32_t j = 0; j < 3; j++){
            // 接触していない影が見つかればfalse
            if(!CheckProjentionCollision(vertices[0], vertices[1], axes[i][j])){
                return false;
            }
        }
    }

    // 分離軸候補を洗い出す(クロス積編)
    for(int32_t i = 0; i < 3; i++){
        for(int32_t j = 0; j < 3; j++){
            Vector3 axis = MyMath::Cross(axes[0][i], axes[1][j]);
            if(MyMath::Length(axis) < 1e-6f) { // ゼロベクトルを無視
                continue;
            }

            // 接触していない影が見つかればfalse
            if(!CheckProjentionCollision(vertices[0], vertices[1], axis)){
                return false;
            }
        }
    }

    // すべての分離軸で接触していたらtrue
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//======================================= AABBとOBBの衝突判定 =======================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool Collision_AABB_OBB(const::AABB& aabb, const::OBB& obb){

    OBB AABB_to_OBB;
    AABB_to_OBB.halfSize = aabb.halfSize;
    AABB_to_OBB.center = aabb.center;

    return Collision_OBB_OBB(AABB_to_OBB, obb);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//======================================= 線分とOBBの衝突判定 =======================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool Collision_Line_OBB(const::Line& line, const::OBB& obb){

    // ローカル頂点
    std::array<Vector3, 8>vertices = obb.GetVertices();

    // 平面を作成
    Quad rect[6] = {
        { vertices[0],vertices[1],vertices[2],vertices[3] },// 手前
        { vertices[5],vertices[4],vertices[7],vertices[6] },// 奥
        { vertices[1],vertices[0],vertices[5],vertices[4] },// 上
        { vertices[2],vertices[3],vertices[6],vertices[7] },// 下
        { vertices[0],vertices[2],vertices[4],vertices[6] },// 左
        { vertices[3],vertices[1],vertices[7],vertices[5] }// 右
    };

    //ここで線分と面で当たり判定
    for(int j = 0; j < 6; j++){
        if(Collision_Quad_Line(rect[j], line)){
            return true;
        }
    }

    return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// お互い静止している線分とOBBの衝突判定
/////////////////////////////////////////////////////////////////////////////////////////////////////
CollisionData CollisionData_Line_OBB(const Line& line, const OBB& obb){
    CollisionData result;

    // OBBの頂点を取得
    std::array<Vector3, 8>obbVertices = obb.GetVertices();

    // 面にする
    Quad quads[6] = {
        { obbVertices[0],obbVertices[1],obbVertices[2],obbVertices[3] },// 手前
        { obbVertices[5],obbVertices[4],obbVertices[7],obbVertices[6] },// 奥
        { obbVertices[1],obbVertices[0],obbVertices[5],obbVertices[4] },// 上
        { obbVertices[2],obbVertices[3],obbVertices[6],obbVertices[7] },// 下
        { obbVertices[0],obbVertices[2],obbVertices[4],obbVertices[6] },// 左
        { obbVertices[3],obbVertices[1],obbVertices[7],obbVertices[5] }// 右
    };

    // 線分と面で当たり判定を行い、当たった場合に法線を取得
    std::vector<Vector3>hitNormals;
    std::vector<Quad*>hitQuads;
    for(int i = 0; i < 6; i++){
        Vector3 hitNormal = Collision_Quad_Line_Normal(quads[i], line);
        if(MyMath::Length(hitNormal) > 0.0f){
            hitNormals.push_back(hitNormal);
            hitQuads.push_back(&quads[i]);
        }
    }

    // 法線情報が無ければ次の判定へ
    if(hitNormals.size() == 0){

        CollisionData data[2] = {
            CollisionData_Point_OBB(line.origin_, obb),
            CollisionData_Point_OBB(line.end_, obb)
        };

        // どちらかの点がOBBに含まれていたら当たっている
        if(data[0].isCollide or data[1].isCollide){
            result.isCollide = true;

            // めり込み深度が小さい方を採用
            if(data[0].collideDepth < data[1].collideDepth){
                result.collideDepth = data[0].collideDepth;
                result.hitNormal = data[0].hitNormal;
            } else{
                result.collideDepth = data[1].collideDepth;
                result.hitNormal = data[1].hitNormal;
            }

        } else{// どちらの点もOBBに含まれていない
            result.isCollide = false;
            return result;
        }


    } else{
        // 当たっている
        result.isCollide = true;

        // 法線を分離軸にしてめり込み深度を計算
        for(int i = 0; i < hitNormals.size(); i++){
            Vector3 axis = MyMath::Normalize(hitNormals[i]);
            CollisionData data;
            CalcProjectionDepth(line, *hitQuads[i], axis, &data);
            if(data.collideDepth >= 0.0f){
                if(data.collideDepth < result.collideDepth){
                    if(MyMath::Dot(line.end_ - line.origin_, axis) < 0.0f){
                        result.collideDepth = data.collideDepth;
                        result.hitNormal = axis;
                    } else{
                        result.collideDepth = 0.0f;
                        result.hitNormal = { 0.0f,0.0f,0.0f };
                    }
                } else{
                    result.collideDepth = 0.0f;
                    result.hitNormal = { 0.0f,0.0f,0.0f };
                }
            } else{
                result.collideDepth = 0.0f;
                result.hitNormal = { 0.0f,0.0f,0.0f };
            }
        }
    }

    return result;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//======================================= 点とOBBの衝突判定 ========================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////
CollisionData CollisionData_Point_OBB(const Vector3& point, const OBB& obb){
    CollisionData result;

    // OBB基準の座標に変換
    Vector3 localPoint = (point - obb.center) * InverseMatrix(RotateMatrix(obb.rotate));

    // OBBに含まれているか確認
    if(localPoint.x > -obb.halfSize.x && localPoint.x < obb.halfSize.x &&
        localPoint.y > -obb.halfSize.y && localPoint.y < obb.halfSize.y &&
        localPoint.z > -obb.halfSize.z && localPoint.z < obb.halfSize.z){
        result.isCollide = true;
    } else{
        result.isCollide = false;
        return result;
    }

    // 押し戻し法線と深さを求める
    Vector3 min = -obb.halfSize;
    Vector3 max = obb.halfSize;
    result.hitPos = point;

    // 押し戻し法線（最近接面の法線を返す）
    float dxMin = std::abs(localPoint.x - min.x);
    float dxMax = std::abs(localPoint.x - max.x);
    float dyMin = std::abs(localPoint.y - min.y);
    float dyMax = std::abs(localPoint.y - max.y);
    float dzMin = std::abs(localPoint.z - min.z);
    float dzMax = std::abs(localPoint.z - max.z);

    float minDist = (std::min)({ dxMin, dxMax, dyMin, dyMax, dzMin, dzMax });
    if(minDist == dxMin) result.hitNormal = { -1.0f, 0.0f, 0.0f };
    else if(minDist == dxMax) result.hitNormal = { 1.0f, 0.0f, 0.0f };
    else if(minDist == dyMin) result.hitNormal = { 0.0f, -1.0f, 0.0f };
    else if(minDist == dyMax) result.hitNormal = { 0.0f, 1.0f, 0.0f };
    else if(minDist == dzMin) result.hitNormal = { 0.0f, 0.0f, -1.0f };
    else if(minDist == dzMax) result.hitNormal = { 0.0f, 0.0f, 1.0f };

    result.collideDepth = minDist + separator; // 衝突深度を最小距離として扱う
    result.hitNormal.value() *= RotateMatrix(obb.rotate); // OBBの回転を考慮して法線を変換

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//======================================= AABB同士の衝突判定 ========================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool Collision_AABB_AABB(const::AABB& aabb1, const::AABB& aabb2){

    Vector3 min[2] = {
        aabb1.center - aabb1.halfSize,
        aabb2.center - aabb2.halfSize
    };

    Vector3 max[2] = {
        aabb1.center + aabb1.halfSize,
        aabb2.center + aabb2.halfSize
    };

    if(min[0].x > max[1].x){ return false; }
    if(max[0].x < min[1].x){ return false; }
    if(min[0].y > max[1].y){ return false; }
    if(max[0].y < min[1].y){ return false; }
    if(min[0].z > max[1].z){ return false; }
    if(max[0].z < min[1].z){ return false; }

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//======================================= 線分とAABBの衝突判定 =======================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool Collision_Line_AABB(const::Line& line, const::AABB& aabb){
    float tmin = 0.0f;
    float tmax = 1.0f;
    Vector3 min = aabb.center - aabb.halfSize;
    Vector3 max = aabb.center + aabb.halfSize;
    Vector3 dif = line.end_ - line.origin_;

    if(dif.x != 0.0f) {
        float tx1 = (min.x - line.origin_.x) / dif.x;
        float tx2 = (max.x - line.origin_.x) / dif.x;

        tmin = (std::max)(tmin, (std::min)(tx1, tx2));
        tmax = (std::min)(tmax, (std::max)(tx1, tx2));
    } else {
        if(line.origin_.x < min.x || line.origin_.x > max.x) {
            return false;
        }
    }

    if(dif.y != 0.0f) {
        float ty1 = (min.y - line.origin_.y) / dif.y;
        float ty2 = (max.y - line.origin_.y) / dif.y;

        tmin = (std::max)(tmin, (std::min)(ty1, ty2));
        tmax = (std::min)(tmax, (std::max)(ty1, ty2));
    } else {
        if(line.origin_.z < min.z || line.origin_.z > max.z) {
            return false;
        }
    }

    if(dif.z != 0.0f) {
        float tz1 = (min.z - line.origin_.z) / dif.z;
        float tz2 = (max.z - line.origin_.z) / dif.z;

        tmin = (std::max)(tmin, (std::min)(tz1, tz2));
        tmax = (std::min)(tmax, (std::max)(tz1, tz2));
    } else {
        if(line.origin_.z < min.z || line.origin_.z > max.z) {
            return false;
        }
    }

    return tmax >= tmin;
}


CollisionData CollisionData_Line_AABB(const Line& line, const AABB& aabb){
    CollisionData result;

    Vector3 start = line.origin_;
    Vector3 end = line.end_;
    Vector3 direction = end - start;

    if(MyMath::Length(direction) < 1e-6f){
        return CollisionData_Point_AABB(start, aabb);
    }

    // 両端がAABB内にあるかをチェック
    bool isStartInside = (start.x > aabb.center.x - aabb.halfSize.x && start.x < aabb.center.x + aabb.halfSize.x &&
        start.y > aabb.center.y - aabb.halfSize.y && start.y < aabb.center.y + aabb.halfSize.y &&
        start.z > aabb.center.z - aabb.halfSize.z && start.z < aabb.center.z + aabb.halfSize.z);

    bool isEndInside = (end.x > aabb.center.x - aabb.halfSize.x && end.x < aabb.center.x + aabb.halfSize.x &&
        end.y > aabb.center.y - aabb.halfSize.y && end.y < aabb.center.y + aabb.halfSize.y &&
        end.z > aabb.center.z - aabb.halfSize.z && end.z < aabb.center.z + aabb.halfSize.z);

    if(isStartInside && isEndInside){
        CollisionData data_origin = CollisionData_Point_AABB(start, aabb);
        CollisionData data_end = CollisionData_Point_AABB(end, aabb);

        // 衝突深度が浅い方を採用
        if(data_origin.collideDepth < data_end.collideDepth){
            return data_origin;
        } else {
            return data_end;
        }
    }

    Vector3 min = aabb.center - aabb.halfSize;
    Vector3 max = aabb.center + aabb.halfSize;

    // 線分の延長線ではなく、[0, 1] 範囲内でのみ交差を判定
    Vector3 invDir = { 1.0f / direction.x, 1.0f / direction.y, 1.0f / direction.z };

    float tMin = 0.0f;
    float tMax = 1.0f;

    // 各軸で交差判定
    for(int i = 0; i < 3; ++i) {
        float t1, t2;
        if(i == 0) { // X軸
            t1 = (min.x - start.x) * invDir.x;
            t2 = (max.x - start.x) * invDir.x;
        } else if(i == 1) { // Y軸
            t1 = (min.y - start.y) * invDir.y;
            t2 = (max.y - start.y) * invDir.y;
        } else { // Z軸
            t1 = (min.z - start.z) * invDir.z;
            t2 = (max.z - start.z) * invDir.z;
        }

        if(t1 > t2) std::swap(t1, t2);
        tMin = (std::max)(tMin, t1);
        tMax = (std::min)(tMax, t2);

        if(tMin > tMax) {
            result.isCollide = false;
            return result;
        }
    }

    result.isCollide = true;

    // 衝突点の計算
    result.hitPos = start + direction * tMin;

    // 押し戻し法線の計算
    Vector3 hitPoint = result.hitPos.value();
    if(std::abs(hitPoint.x - min.x) < 0.0001f) result.hitNormal = { -1.0f, 0.0f, 0.0f };
    else if(std::abs(hitPoint.x - max.x) < 0.0001f) result.hitNormal = { 1.0f, 0.0f, 0.0f };
    else if(std::abs(hitPoint.y - min.y) < 0.0001f) result.hitNormal = { 0.0f, -1.0f, 0.0f };
    else if(std::abs(hitPoint.y - max.y) < 0.0001f) result.hitNormal = { 0.0f, 1.0f, 0.0f };
    else if(std::abs(hitPoint.z - min.z) < 0.0001f) result.hitNormal = { 0.0f, 0.0f, -1.0f };
    else if(std::abs(hitPoint.z - max.z) < 0.0001f) result.hitNormal = { 0.0f, 0.0f, 1.0f };
    else{ result.isCollide = false; }

    // 衝突深度
    result.collideDepth = tMin;

    // 距離と押し戻し比率
    result.distance = tMin;
    result.pushBackRatio_A = tMin / (tMax - tMin);
    result.pushBackRatio_B = (1.0f - result.pushBackRatio_A.value());

    return result;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//======================================= 点とAABBの衝突判定 ========================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////
CollisionData CollisionData_Point_AABB(const Vector3& point, const AABB& aabb){

    CollisionData result;
    Vector3 min = aabb.center - aabb.halfSize;
    Vector3 max = aabb.center + aabb.halfSize;

    // 点がAABB内にあるかをチェック
    if(point.x > min.x && point.x < max.x &&
        point.y > min.y && point.y < max.y &&
        point.z > min.z && point.z < max.z) {
        result.isCollide = true;
        result.hitPos = point;

        // 押し戻し法線（最近接面の法線を返す）
        float dxMin = std::abs(point.x - min.x);
        float dxMax = std::abs(point.x - max.x);
        float dyMin = std::abs(point.y - min.y);
        float dyMax = std::abs(point.y - max.y);
        float dzMin = std::abs(point.z - min.z);
        float dzMax = std::abs(point.z - max.z);

        float minDist = (std::min)({ dxMin, dxMax, dyMin, dyMax, dzMin, dzMax });
        if(minDist == dxMin) result.hitNormal = { -1.0f, 0.0f, 0.0f };
        else if(minDist == dxMax) result.hitNormal = { 1.0f, 0.0f, 0.0f };
        else if(minDist == dyMin) result.hitNormal = { 0.0f, -1.0f, 0.0f };
        else if(minDist == dyMax) result.hitNormal = { 0.0f, 1.0f, 0.0f };
        else if(minDist == dzMin) result.hitNormal = { 0.0f, 0.0f, -1.0f };
        else if(minDist == dzMax) result.hitNormal = { 0.0f, 0.0f, 1.0f };

        result.collideDepth = minDist + separator; // 衝突深度を最小距離として扱う
    } else {
        result.isCollide = false;
    }
    return result;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//======================================= 球とOBBの衝突判定 =========================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool Collision_Sphere_OBB(const::Sphere& sphere, const::OBB& obb){
    // AABBに戻したOBB
    AABB OBBlocal(obb.halfSize * -1.0f, obb.halfSize);

    // OBBの回転を打ち消す行列
    Matrix4x4 inverseOBB = InverseMatrix(AffineMatrix({ 1.0f,1.0f,1.0f }, obb.rotate, obb.center));

    // OBBに合わせて戻した球の作成
    Sphere moved;
    moved = sphere;
    moved.center = Multiply(sphere.center, inverseOBB);

    // 判定
    return Collision_Sphere_AABB(moved, OBBlocal);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
//=================================== 動いているOBBと球の衝突判定 =====================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////

CollisionData CollisionData_MoveOBB_Sphere(Collider* obbCollider, Collider* sphereCollider){
    CollisionData result;

    // 形状が違う場合エラー
    if(obbCollider->GetColliderType() != ColliderType::OBB ||
        sphereCollider->GetColliderType() != ColliderType::Sphere){
        result.error = true;
        return result;
    }

    // 最大AABBが衝突していなけ得れば当たっていない
    if(Collision_AABB_AABB(obbCollider->GetBox(), sphereCollider->GetBox()) == false){
        return result;
    }

    // OBBの情報を取得
    Collider_OBB* convertedOBB = dynamic_cast<Collider_OBB*>(obbCollider);
    OBB obb[2] = {
        convertedOBB->GetOBB(),
        convertedOBB->GetPreOBB()
    };

    // 球の情報を取得
    Collider_Sphere* convertedSphere = dynamic_cast<Collider_Sphere*>(sphereCollider);
    Sphere sphere[2] = {
        convertedSphere->GetSphere(),
        convertedSphere->GetPreSphere()
    };

    // 早期リターンチェック
    float dist[2] = {
        MyMath::Length(obb[1].center - sphere[0].center),
        MyMath::Length(obb[0].center - sphere[0].center)
    };

    float maxRadius = (std::max)({ sphere[0].radius,sphere[1].radius });
    float maxHalfSize = (std::max)({ obb[0].halfSize.x,obb[0].halfSize.y,obb[0].halfSize.z });
    float extension = 1.0f;
    float sum = maxRadius + maxHalfSize + extension;
    if(dist[0] > sum && dist[1] > sum){
        return result;
    }

    // 移動ベクトルを計算
    Vector3 obbMove = obb[1].center - obb[0].center;

    // 静止拡張OBB vs 線分の当たり判定に変換
    OBB forJudgeOBB = obb[0];
    forJudgeOBB.center = obb[1].center;
    forJudgeOBB.halfSize += Vector3(maxRadius, maxRadius, maxRadius);

    // 線分を求める
    Line judgeLine = { sphere[0].center,sphere[0].center - obbMove };

    // 線分とOBBの衝突情報取得
    result = CollisionData_Line_OBB(judgeLine, forJudgeOBB);

    // 衝突していない場合
    if(result.isCollide == false){
        return result;
    }

    // 押し戻し割合を求めるため、質量比を求める
    CalcPushbackRatio(sphereCollider, obbCollider, &result);

    return result;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//=================================== OBBと動いている球の衝突判定 =====================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////
CollisionData CollisionData_OBB_MoveSphere(Collider* obbCollider, Collider* sphereCollider){
    CollisionData result;

    // 形状が違う場合エラー
    if(obbCollider->GetColliderType() != ColliderType::OBB ||
        sphereCollider->GetColliderType() != ColliderType::Sphere){
        result.error = true;
        return result;
    }

    // 最大AABBが衝突していなけ得れば当たっていない
    if(Collision_AABB_AABB(obbCollider->GetBox(), sphereCollider->GetBox()) == false){
        return result;
    }

    // OBBの情報を取得
    Collider_OBB* convertedOBB = dynamic_cast<Collider_OBB*>(obbCollider);
    OBB obb[2] = {
        convertedOBB->GetOBB(),
        convertedOBB->GetPreOBB()
    };

    // 球の情報を取得
    Collider_Sphere* convertedSphere = dynamic_cast<Collider_Sphere*>(sphereCollider);
    Sphere sphere[2] = {
        convertedSphere->GetSphere(),
        convertedSphere->GetPreSphere()
    };

    // 早期リターンチェック
    float dist[2] = {
        MyMath::Length(obb[1].center - sphere[0].center),
        MyMath::Length(obb[0].center - sphere[0].center)
    };

    float maxRadius = (std::max)({ sphere[0].radius,sphere[1].radius });
    float maxHalfSize = (std::max)({ obb[0].halfSize.x,obb[0].halfSize.y,obb[0].halfSize.z });
    float extension = 1.0f;
    float sum = maxRadius + maxHalfSize + extension;
    if(dist[0] > sum && dist[1] > sum){
        return result;
    }

    // 移動ベクトルを計算
    Vector3 sphereMove = sphere[1].center - sphere[0].center;

    // 静止拡張OBB vs 線分の当たり判定に変換
    OBB forJudgeOBB = obb[0];
    forJudgeOBB.halfSize += Vector3(maxRadius, maxRadius, maxRadius);

    // 線分を求める
    Line judgeLine = { sphere[1].center,sphere[0].center };

    // 線分とOBBの衝突情報取得
    result = CollisionData_Line_OBB(judgeLine, forJudgeOBB);

    // 衝突していない場合
    if(result.isCollide == false){
        return result;
    }

    // 押し戻し割合を求めるため、質量比を求める
    CalcPushbackRatio(sphereCollider, obbCollider, &result);

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//======================================= 球とAABBの衝突判定 ========================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool Collision_Sphere_AABB(const::Sphere& sphere, const::AABB& aabb){
    Vector3 closestPos;
    Vector3 min = aabb.center - aabb.halfSize;
    Vector3 max = aabb.center + aabb.halfSize;
    closestPos.x = std::clamp(sphere.center.x, min.x, max.x);
    closestPos.y = std::clamp(sphere.center.y, min.y, max.y);
    closestPos.z = std::clamp(sphere.center.z, min.z, max.z);
    float dist = MyMath::Length(sphere.center - closestPos);

    return dist <= sphere.radius ? true : false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//==========================~============== 球と線分の衝突判定 =======================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool Collision_Sphere_Line(const::Sphere& sphere, const::Line& line){
    Vector3 lineVec = line.end_ - line.origin_;
    Vector3 lineDir = MyMath::Normalize(lineVec);
    Vector3 lineToSphere = sphere.center - line.origin_;
    float t = MyMath::Dot(lineToSphere, lineDir);
    Vector3 closestPoint = line.origin_ + lineDir * t;
    float dist = MyMath::Length(sphere.center - closestPoint);

    return dist <= sphere.radius ? true : false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//========================================= 球同士の衝突判定 =========================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool Collision_Sphere_Sphere(const::Sphere& sphere1, const::Sphere& sphere2){
    float distance = MyMath::Length(sphere1.center - sphere2.center);
    float sumRadius = sphere1.radius + sphere2.radius;
    return distance <= sumRadius;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//===================================== 動いてる球同士の衝突判定 =======================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////
CollisionData CollisionData_MoveSphere_MoveSphere(Collider* sphereCollider1, Collider* sphereCollider2){

    CollisionData result;

    // 形状が違う場合エラー
    if(sphereCollider1->GetColliderType() != ColliderType::Sphere or
        sphereCollider2->GetColliderType() != ColliderType::Sphere){
        return result;
    }

    // 最大AABBが衝突していなけ得れば当たっていない
    if(Collision_AABB_AABB(sphereCollider1->GetBox(), sphereCollider2->GetBox()) == false){
        return result;
    }

    // 球の情報を取得
    Collider_Sphere* convertedSphere1 = dynamic_cast<Collider_Sphere*>(sphereCollider1);
    Collider_Sphere* convertedSphere2 = dynamic_cast<Collider_Sphere*>(sphereCollider2);

    Sphere sphere1[2] = {
        convertedSphere1->GetSphere(),
        convertedSphere1->GetPreSphere()
    };

    Sphere sphere2[2] = {
        convertedSphere2->GetSphere(),
        convertedSphere2->GetPreSphere()
    };

    // MaxRadiusを求める
    float maxRadius[2] = {
        (std::max)(sphere1[0].radius, sphere1[1].radius),
        (std::max)(sphere2[0].radius, sphere2[1].radius)
    };

    // カプセルeにする
    Capsule capsule[2] = {
        { sphere1[0].center, sphere1[1].center, maxRadius[0] },
        { sphere2[0].center, sphere2[1].center, maxRadius[1] }
    };

    // カプセル同士の衝突判定
    result = CollisionData_Capsule_Capusle(capsule[0], capsule[1]);

    if(!result.isCollide){
        return result;
    }
    
    // 押し戻し割合を求めるため、質量比を求める
    CalcPushbackRatio(sphereCollider1, sphereCollider2, &result);

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//======================================= 四角形と線の衝突判定 =======================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool Collision_Quad_Line(const::Quad& rectangle, const::Line& line){

    Triangle triangle[2] = {
        { rectangle.localVertex[0],rectangle.localVertex[1],rectangle.localVertex[2] },
        { rectangle.localVertex[1],rectangle.localVertex[3],rectangle.localVertex[2] }
    };

    if(Collision_Triangle_Line(triangle[0], line)){ return true; }
    if(Collision_Triangle_Line(triangle[1], line)){ return true; }
    return false;
}

Vector3 Collision_Quad_Line_Normal(const::Quad& rectangle, const::Line& line){

    Vector3 normal = { 0.0f,0.0f,0.0f };
    Triangle triangle[2] = {
        { rectangle.localVertex[0],rectangle.localVertex[1],rectangle.localVertex[2] },
        { rectangle.localVertex[1],rectangle.localVertex[3],rectangle.localVertex[2] }
    };

    if(Collision_Triangle_Line(triangle[0], line)){
        normal =
            MyMath::Normalize(MyMath::Cross(
                triangle[0].localVertex[1] - triangle[0].localVertex[0],
                triangle[0].localVertex[2] - triangle[0].localVertex[1]
            ));
    }

    if(Collision_Triangle_Line(triangle[1], line)){
        normal =
            MyMath::Normalize(MyMath::Cross(
                triangle[1].localVertex[1] - triangle[1].localVertex[0],
                triangle[1].localVertex[2] - triangle[1].localVertex[1]
            ));
    }

    return normal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//======================================= 三角形と線の衝突判定 =======================================//
////////////////////////////////////////////////////////////////////////////////////////////////////
bool Collision_Triangle_Line(const Triangle& triangle, const Line& line){
    Vector3 normal = MyMath::Normalize(
        MyMath::Cross(triangle.localVertex[1] - triangle.localVertex[0], triangle.localVertex[2] - triangle.localVertex[1])
    );
    Vector3 dif = line.end_ - line.origin_;
    float dot = MyMath::Dot(normal, dif);

    if(dot == 0.0f){ return false; }// 平行な場合当たらない

    // tを求める
    float distance = MyMath::Dot(triangle.localVertex[0] - line.origin_, normal);
    float t = distance / dot;

    // 衝突点
    Vector3 hitPos = (dif * t) + line.origin_;

    // 三角形の各頂点から見たすべてのクロス積を計算
    Vector3 cross[3] = {
        MyMath::Cross(triangle.localVertex[1] - triangle.localVertex[0],hitPos - triangle.localVertex[1]),
        MyMath::Cross(triangle.localVertex[2] - triangle.localVertex[1],hitPos - triangle.localVertex[2]),
        MyMath::Cross(triangle.localVertex[0] - triangle.localVertex[2],hitPos - triangle.localVertex[0])
    };

    // ひとつでも巻いている向きが違えばfalse
    float triDot[3] = {
        MyMath::Dot(cross[0], cross[1]),
        MyMath::Dot(cross[0], cross[2]),
        MyMath::Dot(cross[1], cross[2])
    };

    if(triDot[0] < 0.0f){ return false; }
    if(triDot[1] < 0.0f){ return false; }
    if(triDot[2] < 0.0f){ return false; }

    // 線の二点が面のどちら側にいるか
    float pointDistance[2] = {
        distance,
        distance - MyMath::Dot(line.end_ - line.origin_,normal),
    };

    if(line.type_ == RAY){

        if(pointDistance[0] >= 0.0f){
            if(pointDistance[1] >= pointDistance[0]){
                return false;
            }
        } else{
            if(pointDistance[1] < pointDistance[0]){
                return false;
            }
        }

    } else if(line.type_ == SEGMENT){

        // 二点がどちらも同じ側にいたら当たっていない
        if(pointDistance[0] > 0.0f){
            if(pointDistance[1] > 0.0f){
                return false;
            }
        }

        if(pointDistance[0] < 0.0f){
            if(pointDistance[1] < 0.0f){
                return false;
            }
        }

    }

    // ここまで来たら衝突
    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//======================================= 線と平面の衝突判定 =======================================//
////////////////////////////////////////////////////////////////////////////////////////////////////
CollisionData CollisionData_Line_Plane(const Line& line, const Quad& plane){
    CollisionData result;

    // 平面の法線
    Vector3 normal = MyMath::Normalize(MyMath::Cross(plane.localVertex[1] - plane.localVertex[0], plane.localVertex[2] - plane.localVertex[1]));

    // 線分の始点から終点へのベクトル
    Vector3 lineVec = line.end_ - line.origin_;

    // 線分の始点から平面までの距離
    float distance = MyMath::Dot(plane.localVertex[0] - line.origin_, normal);

    // 線分の始点から平面までの距離と線分の方向ベクトルの内積
    float dot = MyMath::Dot(normal, lineVec);

    // 線分が平行な場合
    if(dot == 0.0f){
        result.isCollide = false;
        return result;
    }

    // tを求める
    float t = distance / dot;

    // 線分の始点から終点までの距離
    Vector3 hitPos = line.origin_ + lineVec * t;

    // 三角形の各頂点から見たすべてのクロス積を計算
    Vector3 cross[3] = {
        MyMath::Cross(plane.localVertex[1] - plane.localVertex[0], hitPos - plane.localVertex[1]),
        MyMath::Cross(plane.localVertex[2] - plane.localVertex[1], hitPos - plane.localVertex[2]),
        MyMath::Cross(plane.localVertex[0] - plane.localVertex[2], hitPos - plane.localVertex[0])
    };

    // ひとつでも巻いている向きが違えばfalse
    float triDot[3] = {
        MyMath::Dot(cross[0], cross[1]),
        MyMath::Dot(cross[0], cross[2]),
        MyMath::Dot(cross[1], cross[2])
    };

    if(triDot[0] < 0.0f){ result.isCollide = false; }
    if(triDot[1] < 0.0f){ result.isCollide = false; }
    if(triDot[2] < 0.0f){ result.isCollide = false; }

    // 線の二点が面のどちら側にいるか
    float pointDistance[2] = {
        distance,
        distance - MyMath::Dot(line.end_ - line.origin_, normal),
    };

    if(line.type_ == RAY){

        if(pointDistance[0] >= 0.0f){
            if(pointDistance[1] >= pointDistance[0]){
                result.isCollide = false;
            }
        } else{
            if(pointDistance[1] < pointDistance[0]){
                result.isCollide = false;
            }
        }

    } else if(line.type_ == SEGMENT){

        // 二点がどちらも同じ側にいたら当たっていない
        if(pointDistance[0] > 0.0f){
            if(pointDistance[1] > 0.0f){
                result.isCollide = false;
            }
        }

        if(pointDistance[0] < 0.0f){
            if(pointDistance[1] < 0.0f){
                result.isCollide = false;
            }
        }

    } else{// 無限線の場合
        result.isCollide = true;
    }

    // 衝突点
    result.hitPos = hitPos;

    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//==================================== カプセル同士の衝突判定 =======================================//
////////////////////////////////////////////////////////////////////////////////////////////////////
bool Collision::Capsule::Capsule(const::Capsule& capsule1, const::Capsule& capsule2){

    // ラインの作成
    ::Line line[2] = {
        ::Line(capsule1.origin, capsule1.end),
        ::Line(capsule2.origin, capsule2.end)
    };

    // 線分同士の最短距離を取得
    float closestDistance = LineDistance(line[0], line[1]);

    // カプセルの半径の和よりも短ければ衝突している
    return closestDistance <= capsule1.radius + capsule2.radius;
}


// 
CollisionData CollisionData_Capsule_Capusle(const::Capsule& capsule1, const::Capsule& capsule2){

    CollisionData result;

    // ラインの作成
    Line l1 = { capsule1.origin, capsule1.end };
    Line l2 = { capsule2.origin, capsule2.end };

    // 線分同士の最近傍点を二か所求める
    std::array<Vector3, 2> closest = LineClosestPoints(l1, l2);

    float dist = MyMath::Length(closest[0] - closest[1]);
    float sumRadius = capsule1.radius + capsule2.radius;

    // 最近傍点同士の距離が半径の和よりも短ければ衝突している
    if(dist < sumRadius){
        result.isCollide = true;
        result.hitPos = (closest[0] + closest[1]) * 0.5f;
        result.hitNormal = MyMath::Normalize(closest[0] - closest[1]);
        result.collideDepth = sumRadius - dist;

    } else{
        return result;
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//==================================== カプセルと球の衝突判定 =======================================//
////////////////////////////////////////////////////////////////////////////////////////////////////
CollisionData CollisionData_Capsule_Sphere(const::Capsule& capsule, const::Sphere& sphere){
    CollisionData result;

    // カプセルの始点から終点へのベクトル
    Vector3 capsuleVec = capsule.end - capsule.origin;

    // カプセルの始点から球までのベクトル
    Vector3 capsuleToSphere = sphere.center - capsule.origin;

    // カプセルの始点から球までの距離
    float t = MyMath::Dot(capsuleToSphere, capsuleVec) / MyMath::Length(capsuleVec);

    // 線分の始点から最近点までのベクトル
    Vector3 closestPoint = capsule.origin + capsuleVec * t;

    // 最近点から球までのベクトル
    Vector3 closestToSphere = sphere.center - closestPoint;

    // 最近点から球までの距離
    float distance = MyMath::Length(closestToSphere);

    // 最近点がカプセルの始点側にある場合
    if(t < 0.0f){
        distance = MyMath::Length(sphere.center - capsule.origin);
        closestPoint = capsule.origin;
        closestToSphere = sphere.center - capsule.origin;
    }

    // 最近点がカプセルの終点側にある場合
    if(t > MyMath::Length(capsuleVec)){
        distance = MyMath::Length(sphere.center - capsule.end);
        closestPoint = capsule.end;
        closestToSphere = sphere.center - capsule.end;
    }

    // 衝突している場合
    if(distance <= sphere.radius + capsule.radius){
        result.isCollide = true;
        result.hitPos = closestPoint;
        result.hitNormal = MyMath::Normalize(closestToSphere);
        result.collideDepth = distance - (sphere.radius + capsule.radius);
    }

    return result;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//=================================== 動いている球とAABBの衝突判定 ===================================//
////////////////////////////////////////////////////////////////////////////////////////////////////
CollisionData Collision_MoveSphere_AABB(Collider* sphereCollider, Collider* aabbCollider){

    // 衝突データを格納する構造体
    CollisionData collisionData;

    // 例外形状の検出
    if(sphereCollider->GetColliderType() != ColliderType::Sphere){ collisionData.error = true; }
    if(aabbCollider->GetColliderType() != ColliderType::AABB){ collisionData.error = true; }
    if(collisionData.error){ return collisionData; }

    // 最大AABBが衝突していなけ得れば当たっていない
    if(Collision_AABB_AABB(aabbCollider->GetBox(), sphereCollider->GetBox()) == false){
        return collisionData;
    }

    // 球の情報を取得
    Collider_Sphere* convertedSphere = dynamic_cast<Collider_Sphere*>(sphereCollider);
    Sphere sphere[2] = {
        convertedSphere->GetSphere(),
        convertedSphere->GetPreSphere()
    };

    // AABBの情報を取得し、OBBに変換
    Collider_AABB* convertedAABB = dynamic_cast<Collider_AABB*>(aabbCollider);
    AABB aabb = convertedAABB->GetAABB();
    OBB obb;
    obb.center = aabb.center;
    float maxRadius = (std::max)({ sphere[0].radius,sphere[1].radius });
    obb.halfSize = aabb.halfSize + Vector3(maxRadius, maxRadius, maxRadius);
    obb.rotate = { 0.0f,0.0f,0.0f };

    // 移動ベクトルを計算
    Vector3 sphereMove = sphere[1].center - sphere[0].center;
    Line line = { sphere[1].center,sphere[0].center };

    // OBBと線分の当たり判定情報の取得
    collisionData = CollisionData_Line_OBB(line, obb);
    if(collisionData.isCollide == false){ return collisionData; }

    // 押し戻し割合を求めるため、質量比を求める
    CalcPushbackRatio(sphereCollider, aabbCollider, &collisionData);

    return collisionData;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//=================================== 球と動いているAABBの衝突判定 ===================================//
////////////////////////////////////////////////////////////////////////////////////////////////////
CollisionData Collision_Sphere_MoveAABB(Collider* sphereCollider, Collider* aabbCollider){

    // 衝突データを格納する構造体
    CollisionData collisionData;

    // 例外形状の検出
    if(sphereCollider->GetColliderType() != ColliderType::Sphere){ collisionData.error = true; }
    if(aabbCollider->GetColliderType() != ColliderType::AABB){ collisionData.error = true; }
    if(collisionData.error){ return collisionData; }

    // 最大AABBが衝突していなけ得れば当たっていない
    if(Collision_AABB_AABB(aabbCollider->GetBox(), sphereCollider->GetBox()) == false){
        return collisionData;
    }

    // 球の情報を取得
    Collider_Sphere* convertedSphere = dynamic_cast<Collider_Sphere*>(sphereCollider);
    Sphere sphere[2] = { convertedSphere->GetSphere(),convertedSphere->GetPreSphere() };

    // AABBの情報を取得し、OBBに変換
    Collider_AABB* convertedAABB = dynamic_cast<Collider_AABB*>(aabbCollider);
    AABB aabb[2] = { convertedAABB->GetAABB(),convertedAABB->GetPreAABB() };
    OBB obb;
    obb.center = aabb[1].center;
    float maxRadius = (std::max)({ sphere[0].radius,sphere[1].radius });
    obb.halfSize = aabb[0].halfSize + Vector3(maxRadius, maxRadius, maxRadius);
    obb.rotate = { 0.0f,0.0f,0.0f };

    // 移動ベクトルを計算
    Vector3 obbMove = aabb[1].center - aabb[0].center;
    Line line = { sphere[0].center,sphere[0].center - obbMove };

    // OBBと線分の当たり判定情報の取得
    collisionData = CollisionData_Line_OBB(line, obb);
    if(collisionData.isCollide == false){ return collisionData; }

    // 押し戻し割合を求めるため、質量比を求める
    CalcPushbackRatio(sphereCollider, aabbCollider, &collisionData);

    return collisionData;
}