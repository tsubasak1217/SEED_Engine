#include "ColliderAnimationData.h"
#include <SEED.h>

//////////////////////////////////////////////////////////////////////////////
// ImGuiでの編集
//////////////////////////////////////////////////////////////////////////////

void ColliderAnimationData::EditByImGui(const std::string& headerName){
#ifdef _DEBUG
    if(ImGui::CollapsingHeader(headerName.c_str())){
        ImGui::Indent();


        /*----------------------------------*/
        // 要素の挿入
        /*----------------------------------*/
        if(ImGui::CollapsingHeader("Add")){
            ImGui::Indent();

            // 挿入位置の設定
            ImGui::SliderFloat("insertLocation", &insertLocation_, 0.0f, 1.0f);

            // 挿入ボタン
            if(ImGui::Button("Insert")){
                InsertElement(insertLocation_);
            }

            ImGui::Unindent();
        }

        /*-----------------------------------*/
        // 要素の削除
        /*-----------------------------------*/
        if(ImGui::CollapsingHeader("Delete")){
            ImGui::Indent();

            // 削除位置の設定
            ImGui::SliderFloat("deleteLocation", &deleteLocation_, 0.0f, 1.0f);

            // 削除ボタン
            if(ImGui::Button("Delete")){
                DeleteElement(deleteLocation_);
            }

            ImGui::Unindent();
        }


        /*----------------------------------*/ 
        // パラメータの編集
        /*----------------------------------*/
        if(ImGui::CollapsingHeader("Parameters")){
            ImGui::Indent();

            // 時間の設定
            ImGui::InputFloat("lifetime", &duration_);

            /*----------------------------------*/
            // transformの編集
            /*----------------------------------*/
            if(ImGui::CollapsingHeader("Transform")){
                ImGui::Indent();

                // 各indexの要素を表示
                for(int i = 0; i < nodeAnimation->translate.keyframes.size(); i++){

                    if(ImGui::CollapsingHeader(std::string(std::string("index_") + std::to_string(i)).c_str())){
                        ImGui::Indent();

                        //scale
                        ImGui::DragFloat3("scale", &nodeAnimation->scale.keyframes[i].value.x);

                        // rotate
                        Vector3 additionalRotate;
                        if(ImGui::DragFloat3("rotate", &additionalRotate.x)){
                            Quaternion additionalQuat = Quaternion::ToQuaternion(additionalRotate);
                            nodeAnimation->rotate.keyframes[i].value =
                                additionalQuat * nodeAnimation->rotate.keyframes[i].value;
                        }

                        // translate
                        ImGui::DragFloat3("translate", &nodeAnimation->translate.keyframes[i].value.x);

                        ImGui::Unindent();
                    }
                }

                ImGui::Unindent();
            }
            ImGui::Unindent();
        }

        // コライダーの軌道描画
        DrawCollider();

        ImGui::Unindent();
    }
#endif // _DEBUG
}

//////////////////////////////////////////////////////////////////////////////
// 要素の挿入・削除
//////////////////////////////////////////////////////////////////////////////

/*------ 要素の挿入 ------*/
void ColliderAnimationData::InsertElement(float location){

    int32_t index = 0;
    float time = location * duration_;

    /*----------------------*/
    // 時間をもとにindexを求める
    /*----------------------*/
    for(int i = 0; i < nodeAnimation->translate.keyframes.size(); i++){
        if(time >= nodeAnimation->translate.keyframes[i].time){
            index = i + 1;
            break;
        }
    }

    /*----------------------*/
    // 各要素を求め、挿入
    /*----------------------*/
    // translate
    nodeAnimation->translate.keyframes.insert(
        nodeAnimation->translate.keyframes.begin() + index, 
        Keyframe<Vector3>(GetTranslation(time),time)
    );

    // rotate
    nodeAnimation->rotate.keyframes.insert(
        nodeAnimation->rotate.keyframes.begin() + index,
        Keyframe<Quaternion>(GetRotation(time), time)
    );

    // scale
    nodeAnimation->scale.keyframes.insert(
        nodeAnimation->scale.keyframes.begin() + index,
        Keyframe<Vector3>(GetScale(time), time)
    );
}

/*------ 要素の削除 ------*/
void ColliderAnimationData::DeleteElement(float location){
    int32_t index = 0;
    float time = location * duration_;

    /*----------------------*/
    // 時間をもとにindexを求める
    /*----------------------*/
    for(int i = 0; i < nodeAnimation->translate.keyframes.size(); i++){
        if(time >= nodeAnimation->translate.keyframes[i].time){
            index = i;
            break;
        }
    }

    /*----------------------*/
    // 各要素を削除
    /*----------------------*/
    // translate
    nodeAnimation->translate.keyframes.erase(
        nodeAnimation->translate.keyframes.begin() + index
    );

    // rotate
    nodeAnimation->rotate.keyframes.erase(
        nodeAnimation->rotate.keyframes.begin() + index
    );

    // scale
    nodeAnimation->scale.keyframes.erase(
        nodeAnimation->scale.keyframes.begin() + index
    );
}

//////////////////////////////////////////////////////////////////////////////
// コライダーの軌道描画
//////////////////////////////////////////////////////////////////////////////
void ColliderAnimationData::DrawCollider() const{}

//////////////////////////////////////////////////////////////////////////////
// 時間に応じた値を計算
//////////////////////////////////////////////////////////////////////////////
Vector3 ColliderAnimationData::GetScale(float time) const{
    return CalcMomentValue(nodeAnimation->scale.keyframes, time);
}

Quaternion ColliderAnimationData::GetRotation(float time) const{
    return CalcMomentValue(nodeAnimation->rotate.keyframes, time);
}

Vector3 ColliderAnimationData::GetTranslation(float time) const{
    return CalcMomentValue(nodeAnimation->translate.keyframes, time);
}
