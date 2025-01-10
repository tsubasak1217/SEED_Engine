#include "OBB.h"

// OBBの頂点を取得
std::array<Vector3, 8> OBB::GetVertex()const{
    std::array<Vector3, 8> vertex;
    vertex[0] = center + Vector3(-halfSize.x, halfSize.y, -halfSize.z);// 左上前
    vertex[1] = center + Vector3(halfSize.x, halfSize.y, -halfSize.z);// 右上前
    vertex[2] = center + Vector3(-halfSize.x, -halfSize.y, -halfSize.z);// 左下前
    vertex[3] = center + Vector3(halfSize.x, -halfSize.y, -halfSize.z);// 右下前
    vertex[4] = center + Vector3(-halfSize.x, halfSize.y, halfSize.z);// 左上後
    vertex[5] = center + Vector3(halfSize.x, halfSize.y, halfSize.z);// 右上後
    vertex[6] = center + Vector3(-halfSize.x, -halfSize.y, halfSize.z);// 左下後
    vertex[7] = center + Vector3(halfSize.x, -halfSize.y, halfSize.z);// 右下後

    // 回転させる
    for(auto& v : vertex){
        v *= RotateMatrix(rotate);
    }

    return vertex;
}