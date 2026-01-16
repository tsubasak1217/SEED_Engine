#include <SEED/Lib/Structs/AccelerarionField.h>
#include <SEED/Source/SEED.h>

namespace SEED{

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
        Topology::AABB area;
        area.center = (range.min + range.max) * 0.5f;
        area.halfSize = (range.max - range.min) * 0.5f;
        SEED::Instance::DrawAABB(area, { 1.0f,0.0f,0.0f,1.0f });

        // 力の方向を示す矢印の描画
        Vector3 arrowEnd = area.center + acceleration;
        SEED::Instance::DrawLine(area.center, arrowEnd, { 0.0f,1.0f,0.0f,1.0f }, BlendMode::NORMAL);
    }

} // namespace SEED
