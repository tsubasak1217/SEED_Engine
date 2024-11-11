#include "AccelerarionField.h"
#include "SEED.h"

bool AccelerationField::CheckCollision(const Vector3& pos){
    if(range.min.x <= pos.x && pos.x <= range.max.x &&
        range.min.y <= pos.y && pos.y <= range.max.y &&
        range.min.z <= pos.z && pos.z <= range.max.z){
        return true;
    }

    return false;
}

void AccelerationField::DrawRange(){

    Vector3 vertices[8] = {
        range.min,// 手前左下
        { range.min.x,range.max.y,range.min.z },// 手前左上
        { range.min.x,range.max.y,range.max.z },// 手前右上
        { range.min.x,range.min.y,range.max.z },// 手前右下
        { range.max.x,range.min.y,range.min.z },// 奥左下
        { range.max.x,range.max.y,range.min.z },// 奥左上
        range.max,// 奥右上
        { range.max.x,range.min.y,range.max.z }// 奥右下
    };

    // すべての頂点をつないで立方体を描画
    SEED::DrawLine(vertices[0], vertices[1],{1.0f,0.0f,0.0f,1.0f});
    SEED::DrawLine(vertices[1], vertices[2],{1.0f,0.0f,0.0f,1.0f});
    SEED::DrawLine(vertices[2], vertices[3],{1.0f,0.0f,0.0f,1.0f});
    SEED::DrawLine(vertices[3], vertices[0],{1.0f,0.0f,0.0f,1.0f});

    SEED::DrawLine(vertices[4], vertices[5], { 1.0f,0.0f,0.0f,1.0f });
    SEED::DrawLine(vertices[5], vertices[6], { 1.0f,0.0f,0.0f,1.0f });
    SEED::DrawLine(vertices[6], vertices[7], { 1.0f,0.0f,0.0f,1.0f });
    SEED::DrawLine(vertices[7], vertices[4], { 1.0f,0.0f,0.0f,1.0f });

    SEED::DrawLine(vertices[0], vertices[4], { 1.0f,0.0f,0.0f,1.0f });
    SEED::DrawLine(vertices[1], vertices[5], { 1.0f,0.0f,0.0f,1.0f });
    SEED::DrawLine(vertices[2], vertices[6], { 1.0f,0.0f,0.0f,1.0f });
    SEED::DrawLine(vertices[3], vertices[7], { 1.0f,0.0f,0.0f,1.0f });
}
