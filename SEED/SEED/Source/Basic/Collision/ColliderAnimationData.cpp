#include "ColliderAnimationData.h"
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Functions/Math.h>

namespace SEED{
    //////////////////////////////////////////////////////////////////////////////
    // コンストラクタ
    //////////////////////////////////////////////////////////////////////////////
    ColliderAnimationData::ColliderAnimationData(){
        nodeAnimation = SEED::NodeAnimation();
        // 初期値の設定
        duration_ = 0.0f;
        nodeAnimation.translate.keyframes.push_back(Keyframe<Vector3>({ 0.0f,0.0f,0.0f }, 0.0f));
        nodeAnimation.rotate.keyframes.push_back(Keyframe<Quaternion>(Quaternion(), 0.0f));
        nodeAnimation.scale.keyframes.push_back(Keyframe<Vector3>({ 1.0f,1.0f,1.0f }, 0.0f));
    }

    //////////////////////////////////////////////////////////////////////////////
    // ImGuiでの編集
    //////////////////////////////////////////////////////////////////////////////

    void ColliderAnimationData::Edit(const std::string& headerName){

        headerName;
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

                // 追加場所の描画
                DrawCollider(insertLocation_ * duration_);

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
                if(ImGui::Button("Erase")){
                    DeleteElement(deleteLocation_);
                }


                // 削除場所の描画
                DrawCollider(deleteLocation_ * duration_, true);

                ImGui::Unindent();
            }


            /*----------------------------------*/
            // パラメータの編集
            /*----------------------------------*/
            // 時間の設定
            if(ImGui::DragFloat("duration", &duration_, 0.05f)){
                // 各要素の時間を更新
                for(int i = 0; i < nodeAnimation.translate.keyframes.size(); i++){
                    float t = float(i) / float(nodeAnimation.translate.keyframes.size() - 1);
                    nodeAnimation.translate.keyframes[i].time = duration_ * t;
                    nodeAnimation.rotate.keyframes[i].time = duration_ * t;
                    nodeAnimation.scale.keyframes[i].time = duration_ * t;
                }
            };

            // 各indexの要素を表示
            for(int i = 0; i < nodeAnimation.translate.keyframes.size(); i++){
                // ヘッダー名
                std::string indexID = "##" + std::to_string(i);
                std::string transformName = "Transform( " + std::to_string(i) + " )";
                // ヘッダーが開かれたときの処理
                if(ImGui::CollapsingHeader(transformName.c_str())){
                    ImGui::Indent();

                    //scale
                    ImGui::DragFloat3(std::string("scale" + indexID).c_str(), &nodeAnimation.scale.keyframes[i].value.x, 0.005f);

                    // rotate
                    Vector3 additionalRotate;
                    if(ImGui::DragFloat3(std::string("rotate" + indexID).c_str(), &additionalRotate.x, 3.14f * 0.005f)){
                        Vector3 up = nodeAnimation.rotate.keyframes[i].value.MakeUp();
                        Vector3 right = nodeAnimation.rotate.keyframes[i].value.MakeRight();
                        Vector3 forward = nodeAnimation.rotate.keyframes[i].value.MakeForward();

                        Quaternion addRotate =
                            Quaternion::AngleAxis(additionalRotate.x, right) *
                            Quaternion::AngleAxis(additionalRotate.z, forward) *
                            Quaternion::AngleAxis(additionalRotate.y, up);

                        nodeAnimation.rotate.keyframes[i].value = addRotate * nodeAnimation.rotate.keyframes[i].value;

                        SEED::Instance::DrawLine({ 0.0f,0.0f,0.0f }, up * 10.0f, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
                        SEED::Instance::DrawLine({ 0.0f,0.0f,0.0f }, right * 10.0f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
                        SEED::Instance::DrawLine({ 0.0f,0.0f,0.0f }, forward * 10.0f, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
                    }

                    // translate
                    ImGui::DragFloat3(std::string("translate" + indexID).c_str(), &nodeAnimation.translate.keyframes[i].value.x, 0.025f);


                    ImGui::Unindent();
                }
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
        for(int i = 0; i < nodeAnimation.translate.keyframes.size(); i++){
            if(time <= nodeAnimation.translate.keyframes[i].time){
                index = std::clamp(i, 0, (int)nodeAnimation.translate.keyframes.size() - 1);
                break;
            }
        }

        /*----------------------*/
        // 各要素を求め、挿入
        /*----------------------*/
        // translate
        nodeAnimation.translate.keyframes.insert(
            nodeAnimation.translate.keyframes.begin() + index,
            Keyframe<Vector3>(GetTranslation(time), time)
        );

        // rotate
        nodeAnimation.rotate.keyframes.insert(
            nodeAnimation.rotate.keyframes.begin() + index,
            Keyframe<Quaternion>(GetRotation(time), time)
        );

        // scale
        nodeAnimation.scale.keyframes.insert(
            nodeAnimation.scale.keyframes.begin() + index,
            Keyframe<Vector3>(GetScale(time), time)
        );

        // 時間を等間隔に
        for(int i = 0; i < nodeAnimation.translate.keyframes.size(); i++){
            float t = float(i) / float(nodeAnimation.translate.keyframes.size() - 1);
            nodeAnimation.translate.keyframes[i].time = duration_ * t;
            nodeAnimation.rotate.keyframes[i].time = duration_ * t;
            nodeAnimation.scale.keyframes[i].time = duration_ * t;
        }
    }

    /*------ 要素の削除 ------*/
    void ColliderAnimationData::DeleteElement(float location){
        int32_t index = 0;
        float time = location * duration_;

        /*----------------------*/
        // 時間をもとにindexを求める
        /*----------------------*/
        for(int i = 0; i < nodeAnimation.translate.keyframes.size(); i++){
            if(time <= nodeAnimation.translate.keyframes[i].time){
                if(i == nodeAnimation.translate.keyframes.size() - 1 && time == nodeAnimation.translate.keyframes.back().time){
                    index = i;
                } else{
                    index = std::clamp(i, 0, (int)nodeAnimation.translate.keyframes.size() - 1);
                    break;
                }
            }
        }

        /*----------------------*/
        // 各要素を削除
        /*----------------------*/
        // translate
        nodeAnimation.translate.keyframes.erase(
            nodeAnimation.translate.keyframes.begin() + index
        );

        // rotate
        nodeAnimation.rotate.keyframes.erase(
            nodeAnimation.rotate.keyframes.begin() + index
        );

        // scale
        nodeAnimation.scale.keyframes.erase(
            nodeAnimation.scale.keyframes.begin() + index
        );

        if(nodeAnimation.scale.keyframes.size() == 0){ return; }

        /*----------------------*/
        // 時間を更新
        /*----------------------*/

        // 先頭を0に合わせる
        float frontTime = nodeAnimation.translate.keyframes.front().time;
        if(frontTime > 0.0f){
            for(int i = 0; i < nodeAnimation.translate.keyframes.size(); i++){
                nodeAnimation.translate.keyframes[i].time -= frontTime;
                nodeAnimation.rotate.keyframes[i].time -= frontTime;
                nodeAnimation.scale.keyframes[i].time -= frontTime;
            }
        }

        // 時間を更新
        float nextDuration = nodeAnimation.translate.keyframes.back().time;
        for(int i = 0; i < nodeAnimation.translate.keyframes.size(); i++){
            float t = nodeAnimation.translate.keyframes[i].time / nextDuration;
            nodeAnimation.translate.keyframes[i].time = duration_ * t;
            nodeAnimation.rotate.keyframes[i].time = duration_ * t;
            nodeAnimation.scale.keyframes[i].time = duration_ * t;
        }

        // 時間を等間隔に
        for(int i = 0; i < nodeAnimation.translate.keyframes.size(); i++){
            float t = float(i) / float(nodeAnimation.translate.keyframes.size() - 1);
            nodeAnimation.translate.keyframes[i].time = duration_ * t;
            nodeAnimation.rotate.keyframes[i].time = duration_ * t;
            nodeAnimation.scale.keyframes[i].time = duration_ * t;
        }
    }

    //////////////////////////////////////////////////////////////////////////////
    // コライダーの軌道描画
    //////////////////////////////////////////////////////////////////////////////
    void ColliderAnimationData::DrawCollider() const{

        // 一列にまとめる
        std::vector<Vector3> translate;
        std::vector<Quaternion> rotate;
        std::vector<Vector3> scale;

        // 軸モデルの生成
        SEED::Model axis = SEED::Model("DefaultAssets/axis/axis.obj");
        axis.transform_.scale = Vector3(0.3f, 0.3f, 0.3f);

        // 要素を取り出し、行列を計算
        for(int i = 0; i < nodeAnimation.translate.keyframes.size(); i++){
            translate.push_back(nodeAnimation.translate.keyframes[i].value);
            rotate.push_back(nodeAnimation.rotate.keyframes[i].value);
            scale.push_back(nodeAnimation.scale.keyframes[i].value);

            if(parentMat_){
                // 親の行列から取り出した要素を打ち消す行列を作成
                Matrix4x4 worldMat = Methods::Matrix::AffineMatrix(scale[i], rotate[i], translate[i]) * (*parentMat_);

                // 行列から各要素を計算
                translate[i] = Methods::Matrix::ExtractTranslation(worldMat);
                rotate[i] = Methods::Matrix::ExtractQuaternion(worldMat);
                scale[i] = Methods::Matrix::ExtractScale(worldMat);
            }

            // 軸モデルの描画
            axis.transform_.translate = translate[i];
            axis.transform_.scale = scale[i] * axis.transform_.scale;
            axis.transform_.rotate = rotate[i];
            axis.UpdateMatrix();
            axis.Draw();
        }

        // 軌道の描画
        SEED::Instance::DrawSpline(translate, 4, { 1.0f,1.0f,1.0f,1.0f }, false);
    }


    //////////////////////////////////////////////////////////////////////////////
    // 時間指定で
    //////////////////////////////////////////////////////////////////////////////
    void ColliderAnimationData::DrawCollider(float time, bool indexDraw){

        // 軸モデルの生成
        Model axis = Model("DefaultAssets/axis/axis.obj");
        axis.transform_.scale = Vector3(0.3f, 0.3f, 0.3f);
        axis.masterColor_ = Vector4(1.0f, 1.0f, 1.0f, 0.5f);

        // indexDrawなら時間からindexの時間を求める
        if(indexDraw == true){
            for(int i = 0; i < nodeAnimation.translate.keyframes.size(); i++){
                if(time <= nodeAnimation.translate.keyframes[i].time){
                    if(i == nodeAnimation.translate.keyframes.size() - 1 && time == nodeAnimation.translate.keyframes.back().time){
                        time = nodeAnimation.translate.keyframes[i].time;
                        break;
                    } else{
                        int32_t index = std::clamp(i, 0, (int)nodeAnimation.translate.keyframes.size() - 1);
                        time = nodeAnimation.translate.keyframes[index].time;
                        break;
                    }
                }
            }

            axis.masterColor_ = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
        }

        // 軸モデルの描画
        Vector3 translate = GetTranslation(time);
        Quaternion rotate = GetRotation(time);
        Vector3 scale = GetScale(time);

        if(parentMat_){
            // 親の行列を掛け合わせる
            Matrix4x4 resultMat = Methods::Matrix::AffineMatrix(scale, rotate, translate) * (*parentMat_);

            // 行列から各要素を計算
            translate = Methods::Matrix::ExtractTranslation(resultMat);
            rotate = Methods::Matrix::ExtractQuaternion(resultMat);
            scale = Methods::Matrix::ExtractScale(resultMat);
        }

        // 軸モデルの描画
        axis.transform_.translate = translate;
        axis.transform_.scale = scale * axis.transform_.scale;
        axis.transform_.rotate = rotate;
        if(indexDraw){ axis.transform_.scale *= 1.1f; }
        axis.UpdateMatrix();
        axis.Draw();
    }


    //////////////////////////////////////////////////////////////////////////////
    // 時間に応じた値を計算
    //////////////////////////////////////////////////////////////////////////////
    Vector3 ColliderAnimationData::GetScale(float time) const{
        Vector3 result = CalcMomentValue(nodeAnimation.scale.keyframes, time);
        return result;
    }

    Quaternion ColliderAnimationData::GetRotation(float time) const{
        Quaternion result = CalcMomentValue(nodeAnimation.rotate.keyframes, time);
        return result;
    }

    Vector3 ColliderAnimationData::GetTranslation(float time) const{
        // 時間が0ならば0を返す
        if(duration_ == 0.0f){ return Vector3(0.0f, 0.0f, 0.0f); }

        // 時間に応じた値を計算
        std::vector<Vector3> translate;
        float t = time / duration_;
        for(int i = 0; i < nodeAnimation.translate.keyframes.size(); i++){
            translate.push_back(nodeAnimation.translate.keyframes[i].value);
        }
        return Methods::Math::MultiCatmullRom(translate, t);
    }

    //////////////////////////////////////////////////////////////////////////////
    // Jsonデータの取得
    //////////////////////////////////////////////////////////////////////////////
    nlohmann::json ColliderAnimationData::GetJsonData() const{
        nlohmann::json json;

        // 各要素を独立した配列にする
        std::vector<Vector3> translate;
        std::vector<Quaternion> rotate;
        std::vector<Vector3> scale;
        std::vector<float> time;

        // 各要素を取得
        for(int i = 0; i < nodeAnimation.translate.keyframes.size(); i++){
            translate.push_back(nodeAnimation.translate.keyframes[i].value);
            rotate.push_back(nodeAnimation.rotate.keyframes[i].value);
            scale.push_back(nodeAnimation.scale.keyframes[i].value);
            time.push_back(nodeAnimation.translate.keyframes[i].time);
        }

        // アニメーションのデータをまとめていく
        json["duration"] = duration_;
        json["translate"] = translate;
        json["rotate"] = rotate;
        json["scale"] = scale;
        json["time"] = time;

        return json;
    }

    //////////////////////////////////////////////////////////////////////////////
    // Jsonデータの読み込み
    //////////////////////////////////////////////////////////////////////////////
    void ColliderAnimationData::LoadFromJson(const nlohmann::json& jsonData, const Matrix4x4* parentMat){
        // 各要素を取得
        duration_ = jsonData["duration"];
        std::vector<Vector3> translate = jsonData["translate"].get<std::vector<Vector3>>();
        std::vector<Quaternion> rotate = jsonData["rotate"].get<std::vector<Quaternion>>();
        std::vector<Vector3> scale = jsonData["scale"].get<std::vector<Vector3>>();
        std::vector<float> time = jsonData["time"].get<std::vector<float>>();

        // アニメーションデータの設定
        nodeAnimation.translate.keyframes.clear();
        nodeAnimation.rotate.keyframes.clear();
        nodeAnimation.scale.keyframes.clear();
        for(int i = 0; i < translate.size(); i++){
            nodeAnimation.translate.keyframes.push_back(Keyframe<Vector3>(translate[i], time[i]));
            nodeAnimation.rotate.keyframes.push_back(Keyframe<Quaternion>(rotate[i], time[i]));
            nodeAnimation.scale.keyframes.push_back(Keyframe<Vector3>(scale[i], time[i]));
        }

        // 親の行列を設定
        parentMat_ = parentMat;
    }
}