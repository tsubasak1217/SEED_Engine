#include "ColliderEditor.h"

ColliderEditor::~ColliderEditor(){}

////////////////////////////////////////////////////////////
// コライダーの編集
////////////////////////////////////////////////////////////
void ColliderEditor::Edit(){
#ifdef _DEBUG
    // 各コライダー配列ごとにCollapseHeaderみたいなのでをImGuiで見開き編集を行う
    if(ImGui::CollapsingHeader("Collider Editor")){
        // 各コライダーの編集
        for(auto& colliders : colliders_){
            for(auto& collider : colliders){
                // コライダーの編集
                collider->Edit();
            }
        }
    }

#endif // _DEBUG
}
