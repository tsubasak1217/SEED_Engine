#include "Collision.h"
#include "Mymath.h"
#include <vector>
#include <algorithm>

/////////////////////////////////////////////////////////////////////////
//						重複防止のためcpp内で宣言・定義
/////////////////////////////////////////////////////////////////////////
bool CheckProjentionCollision(
    std::vector<Vector3> vertices1, std::vector<Vector3>vertices2,
    const Vector3& axis, CollisionData* pData = nullptr
);
float CalcProjentionDepth(
    std::vector<Vector3> vertices1, std::vector<Vector3>vertices2,
    const Vector3& axis
);
Vector2 GetProjectionRange(std::vector<Vector3> vertices, const Vector3& axis);
bool CalcProjectionDepth(const ::Line& line, const Quad& plane, const Vector3& axis, CollisionData* pData = nullptr);
bool Collision_OBB_OBB(const ::OBB& obb1, const ::OBB& obb2);
bool Collision_AABB_OBB(const ::AABB& aabb, const ::OBB& obb);
bool Collision_Line_OBB(const ::Line& line, const ::OBB& obb);
bool Collision_AABB_AABB(const ::AABB& aabb1, const ::AABB& aabb2);
bool Collision_Line_AABB(const ::Line& line, const ::AABB& aabb);
bool Collision_Sphere_OBB(const ::Sphere& sphere, const ::OBB& obb);
bool Collision_Sphere_AABB(const ::Sphere& sphere, const ::AABB& aabb);
bool Collision_Sphere_Line(const ::Sphere& sphere, const ::Line& line);
bool Collision_Sphere_Sphere(const ::Sphere& sphere1, const ::Sphere& sphere2);
bool Collision_Quad_Line(const ::Quad& rectangle, const ::Line& line);
Vector3 Collision_Quad_Line_Normal(const::Quad& rectangle, const::Line& line);
bool Collision_Triangle_Line(const Triangle& triangle, const Line& line);

/////////////////////////////////////////////////////////////////////////
//						名前空間内の関数の定義
/////////////////////////////////////////////////////////////////////////

namespace Collision{

    namespace OBB{
        bool OBB(const ::OBB& obb1, const ::OBB& obb2){ return Collision_OBB_OBB(obb1, obb2); }
        bool AABB(const ::OBB& obb, const ::AABB& aabb){ return Collision_AABB_OBB(aabb, obb); }
        bool Line(const ::OBB& obb, const ::Line& line){ return Collision_Line_OBB(line, obb); }
        bool Sphere(const::OBB& obb, const::Sphere& sphere){ return Collision_Sphere_OBB(sphere, obb); }
    }

    namespace AABB{
        bool OBB(const ::AABB& aabb, const ::OBB& obb){ return Collision_AABB_OBB(aabb, obb); }
        bool AABB(const ::AABB& aabb1, const ::AABB& aabb2){ return Collision_AABB_AABB(aabb1, aabb2); }
        bool Line(const ::AABB& aabb, const ::Line& line){ return Collision_Line_AABB(line, aabb); }
        bool Sphere(const ::AABB& aabb, const ::Sphere& sphere){ return Collision_Sphere_AABB(sphere, aabb); }
    }

    namespace Line{
        bool OBB(const ::Line& line, const ::OBB& obb){ return Collision_Line_OBB(line, obb); }
        bool AABB(const ::Line& line, const ::AABB& aabb){ return Collision_Line_AABB(line, aabb); }
        bool Sphere(const ::Line& line, const ::Sphere& sphere){ return Collision_Sphere_Line(sphere, line); }
    }

    namespace Sphere{
        bool OBB(const ::Sphere& sphere, const ::OBB& obb){ return Collision_Sphere_OBB(sphere, obb); }
        bool AABB(const ::Sphere& sphere, const ::AABB& aabb){ return Collision_Sphere_AABB(sphere, aabb); }
        bool Line(const ::Sphere& sphere, const ::Line& line){ return Collision_Sphere_Line(sphere, line); }
        bool Sphere(const ::Sphere& sphere1, const ::Sphere& sphere2){ return Collision_Sphere_Sphere(sphere1, sphere2); }
    }

    namespace Quad{
        //bool Line(const ::Quad& rectangle, const ::Line& line){ return false; }
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
//======================================= 射影範囲を取得する関数 =====================================//
/////////////////////////////////////////////////////////////////////////////////////////////////////

Vector2 GetProjectionRange(std::vector<Vector3> vertices, const Vector3& axis){
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
    Vector2 range[2];// xにはmin, yにはmaxが入る
    range[0] = GetProjectionRange(vertices1, axis);
    range[1] = GetProjectionRange(vertices2, axis);

    // すべての影の中のmin,maxを計算
    Vector2 allRange = { (std::min)(range[0].x,range[1].x),(std::max)(range[0].y,range[1].y) };

    // 影の長さの合計値
    float sumLength = (range[0].y - range[0].x) + (range[1].y - range[1].x);
    // すべての影で計算したmin,maxの範囲の長さ
    float subLength = allRange.y - allRange.x;
    float depth = 0.0f;

    // 影が接触していたらtrue
    if(subLength <= sumLength){

        // 衝突データを格納
        if(pData != nullptr){
            // もし衝突対象の影が完全に含まれていたら
            if(range[0].x < range[1].x && range[0].y > range[1].y){
                depth = range[1].y - range[0].x;
            } else if(range[1].x < range[0].x && range[1].y > range[0].y){
                depth = range[1].y - range[0].x;
            } else {
                // そうでない場合は重なっている部分の長さを計算
                depth = sumLength - subLength;
            }

            Vector3 normalAxis = MyMath::Normalize(axis);
            pData->collideDepth = depth + 0.01f;;
        }

        return true;
    } else {
        return false;
    }
}

float CalcProjentionDepth(std::vector<Vector3> vertices1, std::vector<Vector3> vertices2, const Vector3& axis){

    // 射影した範囲を取得
    Vector2 range[2];// xにはmin, yにはmaxが入る
    range[0] = GetProjectionRange(vertices1, axis);
    range[1] = GetProjectionRange(vertices2, axis);

    // すべての影の中のmin,maxを計算
    Vector2 allRange = { (std::min)(range[0].x,range[1].x),(std::max)(range[0].y,range[1].y) };

    // 影の長さの合計値
    float sumLength = (range[0].y - range[0].x) + (range[1].y - range[1].x);
    // すべての影で計算したmin,maxの範囲の長さ
    float subLength = allRange.y - allRange.x;

    // 重なっている部分の長さを計算
    float depth = -1.0f;

    // 影が接触していないとき
    if(subLength > sumLength){
        depth = subLength - sumLength;
    }

    return depth;
}


bool CalcProjectionDepth(const::Line& line, const Quad& plane, const Vector3& axis, CollisionData* pData){

    // 射影した範囲を取得
    Vector2 range[2];// xにはmin, yにはmaxが入る
    std::vector<Vector3> vertices1 = { plane.localVertex[0],plane.localVertex[1],plane.localVertex[2],plane.localVertex[3] };
    std::vector<Vector3> vertices2 = { line.origin_,line.end_ };

    range[0] = GetProjectionRange(vertices1, axis);
    range[1] = GetProjectionRange(vertices2, axis);

    // 当たっているとき
    float depth = 0.0f;
    if(range[1].x < range[0].x){
        // めり込み深度
        depth = range[0].x - range[1].x + 0.01f;

        // 衝突データを格納
        pData->collideDepth = depth;
        pData->pushBackAxis = MyMath::Normalize(axis);
    }

    return depth > 0.0f;
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

    float longSegmrnt = (std::max)({ obb.halfSize.x * 2.0f,obb.halfSize.y * 2.0f,obb.halfSize.z * 2.0f });
    if(MyMath::Length(line.origin_ - obb.center) > longSegmrnt){
        if(MyMath::Length(line.end_ - obb.center) > longSegmrnt){
            return false;
        }
    }


    // ローカル頂点
    std::vector<Vector3>vertices(8);
    vertices[0] = { -obb.halfSize.x,obb.halfSize.y,-obb.halfSize.z };// LT
    vertices[1] = { obb.halfSize.x,obb.halfSize.y,-obb.halfSize.z };// RT
    vertices[2] = { -obb.halfSize.x,-obb.halfSize.y,-obb.halfSize.z };// LB
    vertices[3] = { obb.halfSize.x,-obb.halfSize.y,-obb.halfSize.z };// RB
    vertices[4] = { -obb.halfSize.x,obb.halfSize.y,obb.halfSize.z };// LT
    vertices[5] = { obb.halfSize.x,obb.halfSize.y,obb.halfSize.z };// RT
    vertices[6] = { -obb.halfSize.x,-obb.halfSize.y,obb.halfSize.z };// LB
    vertices[7] = { obb.halfSize.x,-obb.halfSize.y,obb.halfSize.z };// RB

    // ワールド行列を作成
    Matrix4x4 OBBworldMat = AffineMatrix({ 1.0f,1.0f,1.0f }, obb.rotate, obb.center);

    // ワールド座標に変換
    for(int32_t i = 0; i < 8; i++){
        vertices[i] = Multiply(vertices[i], OBBworldMat);
    }

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
//==================================== カプセル同士の衝突判定 =======================================//
////////////////////////////////////////////////////////////////////////////////////////////////////
bool Collision::Capsule::Capsule(const::Capsule& capsule1, const::Capsule& capsule2){

    // ラインの作成
    ::Line line[2] = {
        ::Line(capsule1.origin, capsule1.end),
        ::Line(capsule2.origin, capsule2.end)
    };

    // 線分同士の最短距離を取得
    float closestDistance = MyMath::LineDistance(line[0], line[1]);

    // カプセルの半径の和よりも短ければ衝突している
    return closestDistance <= capsule1.radius + capsule2.radius;
}
