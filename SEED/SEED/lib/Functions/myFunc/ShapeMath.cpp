#include <SEED/Lib/Functions/MyFunc/ShapeMath.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <Environment/Environment.h>

//---------------------- 正三角形 -------------------------

Triangle MakeEqualTriangle(float radius, const Vector4& color){

    float devideTheta = (std::numbers::pi_v<float> *2.0f) / 3.0f;
    float startTheta = std::numbers::pi_v<float> *0.5f;

    return Triangle(
        { radius * std::cos(startTheta),radius * std::sin(startTheta),0.0f },
        { radius * std::cos(startTheta + devideTheta * 2.0f),radius * std::sin(startTheta + devideTheta * 2.0f),0.0f },
        { radius * std::cos(startTheta + devideTheta),radius * std::sin(startTheta + devideTheta),0.0f },
        { 1.0f,1.0f,1.0f },
        { 0.0f,0.0f,0.0f },
        { 0.0f,0.0f,0.0f },
        color
    );
}

Triangle2D MakeEqualTriangle2D(float radius, const Vector4& color){

    float devideTheta = (std::numbers::pi_v<float> *2.0f) / 3.0f;
    float startTheta = std::numbers::pi_v<float> *0.5f * -1.0f;

    return Triangle2D(
        { radius * std::cos(startTheta),radius * std::sin(startTheta) },
        { radius * std::cos(startTheta + devideTheta),radius * std::sin(startTheta + devideTheta) },
        { radius * std::cos(startTheta + devideTheta * 2.0f),radius * std::sin(startTheta + devideTheta * 2.0f) },
        { 1.0f,1.0f },
        0.0f,
        { 0.0f,0.0f },
        color
    );
}

//---------------------- 正四角形 -------------------------

Quad MakeEqualQuad(float radius, const Vector4& color){
    return Quad(
        { -radius, radius ,0.0f },
        { radius, radius,0.0f },
        { -radius, -radius ,0.0f },
        { radius, -radius,0.0f },
        { 1.0f,1.0f,1.0f },
        { 0.0f,0.0f,0.0f },
        { 0.0f,0.0f,0.0f },
        color
    );
}


Quad2D MakeEqualQuad2D(float radius, const Vector4& color){
    return Quad2D(
        { -radius, -radius },
        { radius, -radius },
        { -radius, radius },
        { radius, radius },
        { 1.0f,1.0f },
        0.0f,
        { 0.0f,0.0f },
        color
    );
}

Quad2D MakeBackgroundQuad2D(int32_t layer,const Vector4& color){
    Quad2D quad = MakeQuad2D(kWindowSize,color);
    for(int i = 0; i < 4; ++i){
        quad.localVertex[i].x += kWindowSize.x * 0.5f;
        quad.localVertex[i].y += kWindowSize.y * 0.5f;
    }
    quad.drawLocation = DrawLocation::Back;
    quad.layer = layer;
    return quad;
}

Quad2D MakeFrontQuad2D(int32_t layer, const Vector4& color){
    Quad2D quad = MakeQuad2D(kWindowSize,color);
    for(int i = 0; i < 4; ++i){
        quad.localVertex[i].x += kWindowSize.x * 0.5f;
        quad.localVertex[i].y += kWindowSize.y * 0.5f;
    }
    quad.drawLocation = DrawLocation::Front;
    quad.layer = layer;
    return quad;
}

Quad MakeQuad(const Vector2& size, const Vector4& color, const Vector2& anchorPoint){

    Quad quad = Quad(
        { 0.0f, 0.0f, 0.0f },
        { size.x, 0.0f, 0.0f },
        { 0.0f, -size.y, 0.0f },
        { size.x, -size.y, 0.0f },
        { 1.0f,1.0f,1.0f },
        { 0.0f,0.0f,0.0f },
        { 0.0f,0.0f,0.0f },
        color
    );

    for(int i = 0; i < 4; ++i){
        quad.localVertex[i].x -= size.x * anchorPoint.x;
        quad.localVertex[i].y += size.y * anchorPoint.y;
    }

    return quad;
}

Quad2D MakeQuad2D(const Vector2& size, const Vector4& color, const Vector2& anchorPoint){
    Quad2D quad = Quad2D(
        { 0.0f, 0.0f },
        { size.x, 0.0f },
        { 0.0f, size.y },
        { size.x, size.y },
        { 1.0f,1.0f },
        0.0f,
        { 0.0f,0.0f },
        color
    );

    for(int i = 0; i < 4; ++i){
        quad.localVertex[i].x -= size.x * anchorPoint.x;
        quad.localVertex[i].y -= size.y * anchorPoint.y;// y軸は下方向が正なので逆
    }

    return quad;
}

//-------------- 線分同士の最近傍点を求める関数 ---------------


std::array<Vector3, 2> LineClosestPoints(const Line& l1, const Line& l2){
    // 2直線の方向ベクトル
    Vector3 dir1 = l1.end_ - l1.origin_;
    Vector3 dir2 = l2.end_ - l2.origin_;

    // 2直線の長さ
    float length1 = MyMath::Length(dir1);
    float length2 = MyMath::Length(dir2);

    // 点が同じ場所の場合
    if(length1 == 0.0f){
        if(length2 == 0.0f){
            return { l1.origin_,l2.origin_ };
        } else{
            return { l1.origin_,MyMath::ClosestPoint(l2.origin_,l2.end_,l1.origin_) };
        }
    } else if(length2 == 0.0f){
        if(length1 == 0.0f){
            return { l1.origin_,l2.origin_ };
        } else{
            return { MyMath::ClosestPoint(l1.origin_,l1.end_,l2.origin_),l2.origin_ };
        }
    }

    // 2直線の始点間のベクトル
    Vector3 originVec = l2.origin_ - l1.origin_;

    // 2直線の方向ベクトルの外積
    Vector3 cross = MyMath::Cross(dir1, dir2);
    float crossLength = MyMath::Length(cross);

    // 許容誤差
    const float EPSILON = 1e-6f;

    // 2直線が平行な場合
    if(crossLength < EPSILON){
        return { l1.origin_,l2.origin_ };
    }

    // 最近傍点を計算
    float crossLengthSq = crossLength * crossLength;
    float t1 = MyMath::Dot(MyMath::Cross(originVec, dir2), cross) / crossLengthSq;
    float t2 = MyMath::Dot(MyMath::Cross(originVec, dir1), cross) / crossLengthSq;

    // t1, t2 を [0, 1] に制限
    t1 = std::clamp(t1, 0.0f, 1.0f);
    t2 = std::clamp(t2, 0.0f, 1.0f);

    Vector3 closest1 = l1.origin_ + dir1 * t1;
    Vector3 closest2 = l2.origin_ + dir2 * t2;

    return { closest1, closest2 };
}

// 2直線の距離を求める関数
float LineDistance(const Line& l1, const Line& l2){
    std::array<Vector3, 2> closest = LineClosestPoints(l1, l2);
    return MyMath::Length(closest[1] - closest[0]);
}


//------------------- 最大AABBを求める関数 ------------------
AABB MaxAABB(const AABB& aabb1, const AABB& aabb2){

    Vector3 min[2] = {
        aabb1.center - aabb1.halfSize,
        aabb2.center - aabb2.halfSize
    };

    Vector3 max[2] = {
        aabb1.center + aabb1.halfSize,
        aabb2.center + aabb2.halfSize
    };

    Vector3 mostMin = {
        (std::min)(min[0].x, min[1].x),
        (std::min)(min[0].y, min[1].y),
        (std::min)(min[0].z, min[1].z)
    };

    Vector3 mostMax = {
        (std::max)(max[0].x, max[1].x),
        (std::max)(max[0].y, max[1].y),
        (std::max)(max[0].z, max[1].z)
    };

    Vector3 halfSize = (mostMax - mostMin) * 0.5f;

    return AABB(mostMin + halfSize, halfSize);
}

AABB2D MaxAABB(const AABB2D& aabb1, const AABB2D& aabb2){
    Vector2 min[2] = {
        aabb1.center - aabb1.halfSize,
        aabb2.center - aabb2.halfSize
    };
    Vector2 max[2] = {
        aabb1.center + aabb1.halfSize,
        aabb2.center + aabb2.halfSize
    };
    Vector2 mostMin = {
        (std::min)(min[0].x, min[1].x),
        (std::min)(min[0].y, min[1].y)
    };
    Vector2 mostMax = {
        (std::max)(max[0].x, max[1].x),
        (std::max)(max[0].y, max[1].y)
    };
    Vector2 halfSize = (mostMax - mostMin) * 0.5f;
    return AABB2D(mostMin + halfSize, halfSize);
}

//----------------------BOX頂点を作成する関数-------------------
std::array<Vector3, 8> MakeBox(const Vector3& center, const Vector3& halfSize){
    static std::array<Vector4, 8> vertices = {
        Vector4(-1.0f, 1.0f, -1.0f,1.0f),
        Vector4(1.0f, 1.0f, -1.0f,1.0f),
        Vector4(-1.0f, -1.0f, -1.0f,1.0f),
        Vector4(1.0f, -1.0f, -1.0f,1.0f),
        Vector4(-1.0f, 1.0f, 1.0f,1.0f),
        Vector4(1.0f, 1.0f, 1.0f,1.0f),
        Vector4(-1.0f, -1.0f, 1.0f,1.0f),
        Vector4(1.0f, -1.0f, 1.0f,1.0f)
    };

    std::array<Vector3, 8> boxVertices;
    for(int i = 0; i < 8; ++i){
        boxVertices[i] = Vector3(
            center.x + vertices[i].x * halfSize.x,
            center.y + vertices[i].y * halfSize.y,
            center.z + vertices[i].z * halfSize.z
        );
    }

    return boxVertices;
}
