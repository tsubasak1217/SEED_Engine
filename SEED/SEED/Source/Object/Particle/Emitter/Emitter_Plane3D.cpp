#include <SEED/Source/Object/Particle/Emitter/Emitter.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>
#include "Emitter_Plane3D.h"

Emitter_Plane3D::Emitter_Plane3D(){
}

//
void Emitter_Plane3D::Edit(){
    // 全般の情報
    if(ImGui::CollapsingHeader("General")){
        ImGui::Indent();
        EditGeneral();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }


    // 範囲などの情報
    if(ImGui::CollapsingHeader("Ranges / Parameters")){
        ImGui::Indent();
        EditRangeParameters();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }


    // マテリアルなどの情報
    if(ImGui::CollapsingHeader("Material")){
        ImGui::Indent();
        EditMaterial();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }


    // 頻度などの情報
    if(ImGui::CollapsingHeader("Frequency")){
        ImGui::Indent();
        EditFrequency();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }
}


/*------------------------*/
/*        全般の情報       */
/*------------------------*/
void Emitter_Plane3D::EditGeneral(){
    ImGui::Checkbox("isActive", &isActive);
    ImGui::Checkbox("isBillboard", &isBillboard);
    if(ImGui::Checkbox("isUseRotate", &isUseRotate)){ isBillboard = false; }
    ImGui::Checkbox("isUseGravity", &isUseGravity);
    ImGui::DragFloat3("center", &center.x, 0.05f);
    ImFunc::Combo("emitType", emitType, { "kOnce","kInfinite","kCustom" });
    ImFunc::Combo("particleType", particleType, {"kRadial"});
    ImFunc::Combo("blendMode", blendMode,{ "NONE","MULTIPLY","SUBTRACT","NORMAL","ADD","SCREEN" });
    ImFunc::Combo("CullingMode", cullingMode, { "NONE","FRONT","BACK" });
}


/*------------------------*/
/*      範囲などの情報      */
/*------------------------*/
void Emitter_Plane3D::EditRangeParameters(){
    ImGui::Text("-------- Emit --------");
    ImGui::DragFloat3("emitRange", &emitRange.x, 0.05f);
    ImGui::Text("------- Radius -------");
    ImGui::DragFloat("radiusRange.min", &radiusRange.min, 0.01f, 0.0f, radiusRange.max);
    ImGui::DragFloat("radiusRange.max", &radiusRange.max, 0.01f, radiusRange.min);
    ImGui::Text("------ Direction ------");
    if(ImGui::DragFloat3("baseDirection", &baseDirection.x, 0.01f)){
        baseDirection = MyMath::Normalize(baseDirection);
    };
    ImGui::DragFloat("directionRange", &directionRange, 0.01f, 0.0f, 1.0f);
    ImGui::Text("------- Speed -------");
    ImGui::DragFloat("speedRange.min", &speedRange.min, 0.02f, 0.0f, speedRange.max);
    ImGui::DragFloat("speedRange.max", &speedRange.max, 0.02f, speedRange.min);
    ImGui::Text("------ LifeTime ------");
    ImGui::DragFloat("lifeTimeRange.min", &lifeTimeRange.min, 0.05f, 0.0f, lifeTimeRange.max);
    ImGui::DragFloat("lifeTimeRange.max", &lifeTimeRange.max, 0.05f, lifeTimeRange.min);

    // 回転が適用されている場合のみ
    if(isUseRotate && !isBillboard){
        ImGui::Text("------- Rotate -------");
        ImGui::DragFloat("rotateSpeedRange.min", &rotateSpeedRange.min, 0.01f, 0.0f, rotateSpeedRange.max);
        ImGui::DragFloat("rotateSpeedRange.max", &rotateSpeedRange.max, 0.01f, rotateSpeedRange.min);
    }

    // 重力が適用されている場合のみ
    if(isUseGravity){
        ImGui::Text("------- Gravity -------");
        ImGui::DragFloat("gravity", &gravity, 0.01f);
    }
}


/*------------------------*/
/*     マテリアルなどの情報  */
/*------------------------*/
void Emitter_Plane3D::EditMaterial(){
    // 色の設定
    ImGui::Text("-------- Colors --------");
    if(ImGui::CollapsingHeader("ColorList")){
        ImGui::Indent();

        // 色のリスト
        for(int32_t i = 0; i < (int)colors.size(); ++i){
            std::string colorName = "color_" + std::to_string(i);
            ImGui::ColorEdit4(colorName.c_str(), &colors[i].x);
            // 削除ボタン
            if(ImGui::Button("DeleteColor")){
                if(colors.size() > 1){
                    colors.erase(colors.begin() + i);
                }
            }
        }

        // 追加ボタン
        ImGui::Text("-- Add --");
        if(ImGui::Button("AddColor")){
            colors.emplace_back(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
        }

        ImGui::Unindent();
    }

    // テクスチャの設定
    ImGui::Text("-------- Textures --------");
    if(ImGui::CollapsingHeader("TextureList")){
        ImGui::Indent();

        // エミッターに追加されたテクスチャのリスト
        if(ImGui::CollapsingHeader("EmitTextures")){
            ImGui::Indent();

            // 画像の一覧から選択したものをエミッターのテクスチャリストに追加
            for(int32_t i = 0; i < texturePaths.size(); i++){
                if(ImGui::ImageButton(textureDict[texturePaths[i]], ImVec2(50, 50))){
                    // 消す
                    if(texturePaths.size() > 1){
                        texturePaths.erase(
                            std::remove(texturePaths.begin(), texturePaths.end(), texturePaths[i]),
                            texturePaths.end()
                        );
                    }
                }
            }

            ImGui::Unindent();
        }

        // すべてのテクスチャのリスト
        if(ImGui::CollapsingHeader("Liblaly")){
            ImGui::Indent();

            // 画像の一覧から選択したものをエミッターのテクスチャリストに追加
            for(auto& texture : textureDict){
                if(ImGui::ImageButton(texture.second, ImVec2(50, 50))){
                    texturePaths.push_back(texture.first);
                }
            }

            ImGui::Unindent();
        }

        ImGui::Unindent();
    }
}


/*------------------------*/
/*      頻度などの情報      */
/*------------------------*/
void Emitter_Plane3D::EditFrequency(){
    ImGui::DragFloat("emitInterval", &interval, 0.01f, 0.0f);
    ImGui::DragInt("numEmitEvery", &numEmitEvery, 1, 0, 100);
    if(emitType == EmitType::kCustom){
        ImGui::DragInt("kMaxEmitCount", &kMaxEmitCount, 1);
    }
}


///////////////////////////////////////////////////////////
// 出力
///////////////////////////////////////////////////////////
nlohmann::json Emitter_Plane3D::ExportToJson(){
    nlohmann::json j;

    // 全般の情報
    j["emitterType"] = "Emitter_Plane3D";
    j["isActive"] = isActive;
    j["isBillboard"] = isBillboard;
    j["isUseRotate"] = isUseRotate;
    j["isUseGravity"] = isUseGravity;
    j["emitType"] = (int)emitType;
    j["particleType"] = (int)particleType;
    j["blendMode"] = (int)blendMode;
    j["CullingMode"] = (int)cullingMode;
    j["center"] = { center.x, center.y, center.z };

    // 範囲などの情報
    j["emitRange"] = { emitRange.x, emitRange.y, emitRange.z };
    j["radiusRange"] = { radiusRange.min, radiusRange.max };
    j["baseDirection"] = { baseDirection.x, baseDirection.y, baseDirection.z };
    j["directionRange"] = directionRange;
    j["speedRange"] = { speedRange.min, speedRange.max };
    j["rotateSpeedRange"] = { rotateSpeedRange.min, rotateSpeedRange.max };
    j["lifeTimeRange"] = { lifeTimeRange.min, lifeTimeRange.max };
    j["gravity"] = gravity;

    // 発生頻度などの情報
    j["interval"] = interval;
    j["numEmitEvery"] = numEmitEvery;
    j["kMaxEmitCount"] = kMaxEmitCount;

    // 色の情報
    for(auto& color : colors){
        j["colors"].push_back({ color.x, color.y, color.z, color.w });
    }

    // テクスチャの情報
    for(auto& textureHandle : texturePaths){
        j["textureHandles"].push_back(textureHandle);
    }

    return j;
}

///////////////////////////////////////////////////////////
// 読み込み
///////////////////////////////////////////////////////////
void Emitter_Plane3D::LoadFromJson(const nlohmann::json& j){

    // 全般の情報
    isActive = j["isActive"];
    isBillboard = j["isBillboard"];
    isUseRotate = j["isUseRotate"];
    isUseGravity = j["isUseGravity"];
    emitType = (EmitType)j["emitType"];
    particleType = (ParticleType)j["particleType"];
    blendMode = (BlendMode)j["blendMode"];
    cullingMode = j["CullingMode"];
    center = Vector3(
        j["center"][0], j["center"][1], j["center"][2]
    );

    // 範囲などの情報
    emitRange = Vector3(
        j["emitRange"][0], j["emitRange"][1], j["emitRange"][2]
    );
    radiusRange = Range1D(
        j["radiusRange"][0], j["radiusRange"][1]
    );
    baseDirection = Vector3(
        j["baseDirection"][0], j["baseDirection"][1], j["baseDirection"][2]
    );
    directionRange = j["directionRange"];
    speedRange = Range1D(
        j["speedRange"][0], j["speedRange"][1]
    );
    rotateSpeedRange = Range1D(
        j["rotateSpeedRange"][0], j["rotateSpeedRange"][1]
    );
    lifeTimeRange = Range1D(
        j["lifeTimeRange"][0], j["lifeTimeRange"][1]
    );
    gravity = j["gravity"];


    // 発生頻度などの情報
    interval = j["interval"];
    numEmitEvery = j["numEmitEvery"];
    kMaxEmitCount = j["kMaxEmitCount"];

    // 色の情報
    colors.clear();
    for(auto& color : j["colors"]){
        colors.emplace_back(Vector4(color[0], color[1], color[2], color[3]));
    }

    // テクスチャの情報
    texturePaths.clear();
    for(auto& textureHandle : j["textureHandles"]){
        texturePaths.push_back(textureHandle);
    }
}

