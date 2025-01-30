#include "OBB.h"

// OBBの頂点を取得
std::array<Vector3, 8> OBB::GetVertices()const{
    std::array<Vector3, 8> vertex;
    vertex[0] = Vector3(-halfSize.x, halfSize.y, -halfSize.z);// 左上前
    vertex[1] = Vector3(halfSize.x, halfSize.y, -halfSize.z);// 右上前
    vertex[2] = Vector3(-halfSize.x, -halfSize.y, -halfSize.z);// 左下前
    vertex[3] = Vector3(halfSize.x, -halfSize.y, -halfSize.z);// 右下前
    vertex[4] = Vector3(-halfSize.x, halfSize.y, halfSize.z);// 左上後
    vertex[5] = Vector3(halfSize.x, halfSize.y, halfSize.z);// 右上後
    vertex[6] = Vector3(-halfSize.x, -halfSize.y, halfSize.z);// 左下後
    vertex[7] = Vector3(halfSize.x, -halfSize.y, halfSize.z);// 右下後

    // 回転させる
    for(auto& v : vertex){
        v *= RotateMatrix(rotate);
        v += center;
    }

    return vertex;
}