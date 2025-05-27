#include <SEED/Source/Object/Particle/Emitter/Emitter.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>
#include "Emitter_Plane.h"

Emitter_Plane::Emitter_Plane(){
}

//
void Emitter_Plane::Edit(){
    // 全般の情報
    if(ImGui::CollapsingHeader("全般")){
        ImGui::Indent();
        EditGeneral();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }


    // 範囲などの情報
    if(ImGui::CollapsingHeader("範囲 / パラメーター")){
        ImGui::Indent();
        EditRangeParameters();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }

    // イージング関数の情報
    if(ImGui::CollapsingHeader("イージング")){
        ImGui::Indent();
        EditEaseType();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }

    // マテリアルなどの情報
    if(ImGui::CollapsingHeader("マテリアル")){
        ImGui::Indent();
        EditMaterial();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }


    // 頻度などの情報
    if(ImGui::CollapsingHeader("発生頻度・個数")){
        ImGui::Indent();
        EditFrequency();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }
}


/*------------------------*/
/*        全般の情報       */
/*------------------------*/
void Emitter_Plane::EditGeneral(){
    ImGui::Checkbox("アクティブ", &isActive);
    ImGui::Checkbox("ビルボードするか", &isBillboard);
    if(ImGui::Checkbox("回転するか", &isUseRotate)){ isBillboard = false; }
    ImGui::Checkbox("重力を使用するか", &isUseGravity);
    ImFunc::Combo("particleType", particleType, {"kRadial"});
    ImFunc::Combo("ブレンド", blendMode,{ "NONE","MULTIPLY","SUBTRACT","NORMAL","ADD","SCREEN" });
    ImFunc::Combo("カリング設定", cullingMode, { "なし","前面","背面" });
    ImFunc::Combo("発生タイプ", emitType, { "一度のみ","ずっと","指定回数" });
    if(emitType == EmitType::kCustom){
        ImGui::DragInt("発生回数", &kMaxEmitCount, 1);
    }
}


/*------------------------*/
/*      範囲などの情報      */
/*------------------------*/
void Emitter_Plane::EditRangeParameters(){
    ImGui::Text("-------- 出現 --------");
    ImGui::DragFloat3("中心座標", &center.x, 0.05f);
    ImGui::DragFloat3("発生範囲", &emitRange.x, 0.05f);
    ImGui::Text("------- 半径 -------");
    ImGui::DragFloat("最小", &radiusRange.min, 0.01f, 0.0f, radiusRange.max);
    ImGui::DragFloat("最大", &radiusRange.max, 0.01f, radiusRange.min);
    ImGui::Text("------- スケール -------");
    ImGui::DragFloat2("最小", &scaleRange.min.x, 0.005f, 0.0f, scaleRange.max.x);
    ImGui::DragFloat2("最大", &scaleRange.max.x, 0.005f, scaleRange.min.x);
    ImGui::Text("------ 向き ------");
    if(ImGui::DragFloat3("基礎となる方向", &baseDirection.x, 0.01f)){
        baseDirection = MyMath::Normalize(baseDirection);
    };
    ImGui::DragFloat("方向のばらけ具合", &directionRange, 0.01f, 0.0f, 1.0f);
    ImGui::Text("------- 速さ -------");
    ImGui::DragFloat("最小", &speedRange.min, 0.02f, 0.0f, speedRange.max);
    ImGui::DragFloat("最大", &speedRange.max, 0.02f, speedRange.min);
    ImGui::Text("------ 寿命 ------");
    ImGui::DragFloat("最短", &lifeTimeRange.min, 0.05f, 0.0f, lifeTimeRange.max);
    ImGui::DragFloat("最長", &lifeTimeRange.max, 0.05f, lifeTimeRange.min);

    // 回転が適用されている場合のみ
    if(isUseRotate && !isBillboard){
        ImGui::Text("------- 回転 -------");
        ImGui::DragFloat("最小回転速度", &rotateSpeedRange.min, 0.01f, 0.0f, rotateSpeedRange.max);
        ImGui::DragFloat("最大回転速度", &rotateSpeedRange.max, 0.01f, rotateSpeedRange.min);
    }

    // 重力が適用されている場合のみ
    if(isUseGravity){
        ImGui::Text("------- 重力 -------");
        ImGui::DragFloat("重力", &gravity, 0.01f);
    }
}


/*------------------------*/
/*      イージング関数の情報  */
/*------------------------*/
void Emitter_Plane::EditEaseType(){
    ImGui::Text("-------- イージング形式 --------");
    ImGui::Checkbox("サイズ減衰するか", &enableSizeDecay);
    ImGui::Checkbox("透明度減衰するか", &enableAlphaDecay);
    ImFunc::Combo("速度", velocityEaseType_, Easing::names,IM_ARRAYSIZE(Easing::names));
    ImFunc::Combo("回転", rotateEaseType_, Easing::names, IM_ARRAYSIZE(Easing::names));
    ImFunc::Combo("減衰", decayEaseType_, Easing::names, IM_ARRAYSIZE(Easing::names));
}


/*------------------------*/
/*     マテリアルなどの情報  */
/*------------------------*/
void Emitter_Plane::EditMaterial(){
    // 色の設定
    ImGui::Text("-------- 色 --------");
    if(ImGui::CollapsingHeader("出現色の一覧")){
        ImGui::Indent();

        // 色のリスト
        for(int32_t i = 0; i < (int)colors.size(); ++i){
            std::string colorName = "color_" + std::to_string(i);
            ImGui::ColorEdit4(colorName.c_str(), &colors[i].x);
            // 削除ボタン
            if(ImGui::Button("削除")){
                if(colors.size() > 1){
                    colors.erase(colors.begin() + i);
                }
            }
        }

        // 追加ボタン
        ImGui::Text("-- 色の追加 --");
        if(ImGui::Button("追加")){
            colors.emplace_back(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
        }

        ImGui::Unindent();
    }

    // テクスチャの設定
    ImGui::Text("-------- テクスチャ --------");
    if(ImGui::CollapsingHeader("テクスチャ一覧")){
        ImGui::Indent();

        // エミッターに追加されたテクスチャのリスト
        if(ImGui::CollapsingHeader("追加されたテクスチャ")){
            ImGui::Indent();

            // 画像の一覧から選択したものをエミッターのテクスチャリストに追加
            for(int32_t i = 0; i < texturePaths.size(); i++){
                if(ImGui::ImageButton(texturePaths[i].c_str(), textureDict[texturePaths[i]], ImVec2(50, 50))){
                    // 消す
                    if(texturePaths.size() > 1){
                        texturePaths.erase(
                            std::remove(texturePaths.begin(), texturePaths.end(), texturePaths[i]),
                            texturePaths.end()
                        );
                    }
                }

                if((i + 1) % 5 != 0){
                    ImGui::SameLine();
                }
            }

            ImGui::NewLine();
            ImGui::Unindent();
        }

        // すべてのテクスチャのリスト
        if(ImGui::CollapsingHeader("ライブラリ")){
            ImGui::Indent();

            // 画像の一覧から選択したものをエミッターのテクスチャリストに追加
            int i = 0;
            for(auto& texture : textureDict){
                if(ImGui::ImageButton(texture.first.c_str(), texture.second, ImVec2(50, 50))){
                    texturePaths.push_back(texture.first);
                }

                if((i + 1) % 5 != 0){
                    ImGui::SameLine();
                }
                i++;
            }

            ImGui::NewLine();
            ImGui::Unindent();
        }

        ImGui::Unindent();
    }
}


/*------------------------*/
/*      頻度などの情報      */
/*------------------------*/
void Emitter_Plane::EditFrequency(){
    ImGui::DragFloat("発生間隔", &interval, 0.01f, 0.0f);
    ImGui::DragInt("一度に発生する数", &numEmitEvery, 1, 0, 100);
}


///////////////////////////////////////////////////////////
// 出力
///////////////////////////////////////////////////////////
nlohmann::json Emitter_Plane::ExportToJson(){
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
    j["scaleRange"] = { {scaleRange.min.x, scaleRange.min.y}, {scaleRange.max.x, scaleRange.max.y} };

    // イージング関数の情報
    j["velocityEaseType"] = (int)velocityEaseType_;
    j["rotateEaseType"] = (int)rotateEaseType_;
    j["decayEaseType"] = (int)decayEaseType_;

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
void Emitter_Plane::LoadFromJson(const nlohmann::json& j){

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
    emitRange = Vector3(j["emitRange"]);
    radiusRange = Range1D(j["radiusRange"][0], j["radiusRange"][1]);
    baseDirection = Vector3(j["baseDirection"]);
    directionRange = j["directionRange"];
    speedRange = Range1D(j["speedRange"][0], j["speedRange"][1]);
    rotateSpeedRange = Range1D(j["rotateSpeedRange"][0], j["rotateSpeedRange"][1]);
    lifeTimeRange = Range1D(j["lifeTimeRange"][0], j["lifeTimeRange"][1]);
    gravity = j["gravity"];
    scaleRange = Range2D(j["scaleRange"][0],j["scaleRange"][1]);

    // イージング関数の情報
    //velocityEaseType_ = (Easing::Type)j["velocityEaseType"];
    //rotateEaseType_ = (Easing::Type)j["rotateEaseType"];
    //decayEaseType_ = (Easing::Type)j["decayEaseType"];

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

