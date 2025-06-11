#include <SEED/Lib/Structs/AccelerarionField.h>
#include <SEED/Source/SEED.h>

bool AccelerationField::CheckCollision(const Vector3& pos){
    if(range.min.x <= pos.x && pos.x <= range.max.x &&
        range.min.y <= pos.y && pos.y <= range.max.y &&
        range.min.z <= pos.z && pos.z <= range.max.z){
        return true;
    }

    return false;
}

void AccelerationField::Draw() const{
    // 範囲の描画
    AABB area;
    area.center = (range.min + range.max) * 0.5f;
    area.halfSize = (range.max - range.min) * 0.5f;
    SEED::DrawAABB(area, { 1.0f,0.0f,0.0f,1.0f });

    // 力の方向を示す矢印の描画
    Vector3 arrowEnd = area.center + acceleration;
    SEED::DrawLine(area.center, arrowEnd, { 0.0f,1.0f,0.0f,1.0f }, BlendMode::NORMAL);
}
