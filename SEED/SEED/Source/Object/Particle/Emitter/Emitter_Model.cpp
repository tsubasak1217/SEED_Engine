#include <SEED/Source/Object/Particle/Emitter/Emitter.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>
#include <SEED/Source/Manager/ModelManager/ModelManager.h>
#include "Emitter_Model.h"

Emitter_Model::Emitter_Model() : Emitter_Base(){
}

//
void Emitter_Model::Edit(){

    static std::string label = "";

    // 全般の情報
    label = "全般" + idTag_;
    if(ImGui::CollapsingHeader(label.c_str())){
        ImGui::Indent();
        EditGeneral();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }


    // 範囲などの情報
    label = "範囲 / パラメーター" + idTag_;
    if(ImGui::CollapsingHeader(label.c_str())){
        ImGui::Indent();
        EditRangeParameters();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }

    // イージング関数の情報
    label = "出現/消滅パラメーター・イージング" + idTag_;
    if(ImGui::CollapsingHeader(label.c_str())){
        ImGui::Indent();
        EditEaseType();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }

    // マテリアルなどの情報
    label = "マテリアル・形状・描画設定" + idTag_;
    if(ImGui::CollapsingHeader(label.c_str())){
        ImGui::Indent();
        EditMaterial();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }


    // 頻度などの情報
    label = "発生頻度・個数" + idTag_;
    if(ImGui::CollapsingHeader(label.c_str())){
        ImGui::Indent();
        EditFrequency();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }
}


/*------------------------*/
/*        全般の情報       */
/*------------------------*/
void Emitter_Model::EditGeneral(){
    ImGui::Checkbox("アクティブ", &isActive);
    ImFunc::Combo("particleType", particleType, { "kRadial" });
    ImFunc::Combo("発生タイプ", emitType, { "一度のみ","ずっと","指定回数" });
    if(emitType == EmitType::kCustom){
        ImGui::DragInt("発生回数", &kMaxEmitCount, 1);
    }
}


/*------------------------*/
/*      範囲などの情報      */
/*------------------------*/
void Emitter_Model::EditRangeParameters(){
    ImGui::Text("-------- 出現 --------");
    ImGui::DragFloat3("中心座標", &center.x, 0.05f);
    ImGui::DragFloat3("発生範囲", &emitRange.x, 0.05f);
    
    ImGui::Text("------- 半径 -------");
    ImGui::DragFloat("最小半径", &radiusRange.min, 0.01f, 0.0f, radiusRange.max);
    ImGui::DragFloat("最大半径", &radiusRange.max, 0.01f, radiusRange.min);
    
    ImGui::Text("------- スケール -------");
    ImGui::DragFloat3("最小倍率(x,y,x)", &scaleRange.min.x, 0.005f, 0.0f, scaleRange.max.x);
    ImGui::DragFloat3("最大倍率(x,y,x)", &scaleRange.max.x, 0.005f, scaleRange.min.x);
    
    
    ImGui::Text("------- 移動 -------");
    ImGui::Checkbox("目標位置を設定するか", &isSetGoalPosition);
    if(isSetGoalPosition){
        ImGui::Checkbox("目標位置で終了するか", &isEndWithGoalPosition);
        ImGui::DragFloat3("ゴール位置", &goalPosition.x, 0.05f);
    } else{
        isEndWithGoalPosition = false;
        if(ImGui::DragFloat3("基礎となる方向", &baseDirection.x, 0.01f)){
            baseDirection = MyMath::Normalize(baseDirection);
        };
    }
    if(!isEndWithGoalPosition){
        ImGui::DragFloat("方向のばらけ具合", &directionRange, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("最低速度", &speedRange.min, 0.02f, 0.0f, speedRange.max);
        ImGui::DragFloat("最高速度", &speedRange.max, 0.02f, speedRange.min);
    }
    
    
    ImGui::Text("------ 寿命 ------");
    ImGui::DragFloat("最短", &lifeTimeRange.min, 0.05f, 0.0f, lifeTimeRange.max);
    ImGui::DragFloat("最長", &lifeTimeRange.max, 0.05f, lifeTimeRange.min);

    
    ImGui::Text("------- 回転 -------");
    ImGui::Checkbox("ビルボードするか", &isBillboard);
    ImGui::Checkbox("回転の初期化値をランダムにするか", &isRoteteRandomInit_);
    ImGui::Checkbox("回転するか", &isUseRotate);
    if(isUseRotate){
        ImGui::Checkbox("回転軸を指定するか", &useRotateDirection);
        if(useRotateDirection){
            ImGui::DragFloat3("回転軸", &rotateDirection.x, 0.01f);
            rotateDirection = MyMath::Normalize(rotateDirection);
        }
        ImGui::DragFloat("最小回転速度", &rotateSpeedRange.min, 0.01f, -10000.0f, rotateSpeedRange.max);
        ImGui::DragFloat("最大回転速度", &rotateSpeedRange.max, 0.01f, rotateSpeedRange.min);
    }

    
    ImGui::Text("------- 重力 -------");
    ImGui::Checkbox("重力を使用するか", &isUseGravity);
    if(isUseGravity){
        ImGui::DragFloat("重力", &gravity, 0.01f);
    }
}


/*------------------------*/
/*      イージング関数の情報  */
/*------------------------*/
void Emitter_Model::EditEaseType(){
    ImGui::Text("-------- 出現・消失パラメーター --------");
    ImGui::DragFloat3("出現時のスケール", &kInScale.x, 0.01f);
    ImGui::DragFloat3("消失時のスケール", &kOutScale.x, 0.01f);
    ImGui::SliderFloat("出現時のアルファ値", &kInAlpha, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("消失時のアルファ値", &kOutAlpha, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("最大に到達する時間", &maxTimePoint, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("最大を維持する時間割合", &maxTimeRate, 0.0f, 1.0f, "%.2f");
    ImGui::Text("-------- イージング関数 --------");
    ImFunc::Combo("速度", velocityEaseType_, Easing::names, IM_ARRAYSIZE(Easing::names));
    ImFunc::Combo("回転", rotateEaseType_, Easing::names, IM_ARRAYSIZE(Easing::names));
    ImFunc::Combo("出現", enterEaseType_, Easing::names, IM_ARRAYSIZE(Easing::names));
    ImFunc::Combo("消滅", exitEaseType_, Easing::names, IM_ARRAYSIZE(Easing::names));
}


/*------------------------*/
/*     マテリアルなどの情報  */
/*------------------------*/
void Emitter_Model::EditMaterial(){

    static std::string label = "";

    // BlendMode, CullingMode,LightingTypeの設定
    ImGui::Text("-------- 描画設定 --------");
    ImFunc::Combo("ブレンドモード", blendMode, { "NONE","MULTIPLY","SUBTRACT","NORMAL","ADD","SCREEN" });
    ImFunc::Combo("ライティング", lightingType_, { "なし","ランバート","ハーフランバート" });
    ImFunc::Combo("カリング設定", cullingMode, { "なし","前面","背面" }, 1);

    // 色の設定
    ImGui::Text("-------- 色 --------");
    if(ImGui::CollapsingHeader("出現色の一覧")){
        ImGui::Indent();

        // 色のリスト
        for(int32_t i = 0; i < (int)colors.size(); ++i){
            std::string colorName = "color_" + std::to_string(i);
            ImGui::ColorEdit4(colorName.c_str(), &colors[i].x);
            // 削除ボタン
            label = "削除" + idTag_;
            if(ImGui::Button(label.c_str())){
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
    if(ImGui::CollapsingHeader("テクスチャ設定")){
        ImGui::Indent();

        // エミッターに追加されたテクスチャのリスト
        ImGui::Checkbox("モデルのデフォルトテクスチャ使用する", &useDefaultTexture);
        if(!useDefaultTexture){
            if(ImGui::CollapsingHeader("カスタムテクスチャ")){
                ImGui::Indent();

                // 画像の一覧から選択したものをエミッターのテクスチャリストに追加
                for(int32_t i = 0; i < texturePaths.size(); i++){

                    label = texturePaths[i].c_str() + idTag_;
                    if(ImGui::ImageButton(label.c_str(), textureDict[texturePaths[i]], ImVec2(50, 50))){
                        // 消す
                        if(texturePaths.size() > 1){
                            textureSet.erase(texturePaths[i]);
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
                    label = texture.first + idTag_ + '1';
                    if(ImGui::ImageButton(label.c_str(), texture.second, ImVec2(50, 50))){
                        if(textureSet.find(texture.first) == textureSet.end()){
                            // テクスチャを追加
                            textureSet.insert(texture.first);
                            texturePaths.push_back(texture.first);
                        }
                    }

                    if((i + 1) % 5 != 0){
                        ImGui::SameLine();
                    }
                    i++;
                }

                ImGui::NewLine();
                ImGui::Unindent();
            }
        }
        ImGui::Unindent();
    }

    // モデルの設定
    ImGui::Text("-------- モデル --------");
    if(ImGui::CollapsingHeader("モデル一覧")){
        ImGui::Indent();
        // モデルの一覧から選択したものをエミッターのモデルリストに追加
        static std::vector<std::string> modelPaths;
        modelPaths = ModelManager::GetModelNames();
        ImGui::Text("現在のモデル:%s", emitModelFilePath_.c_str());
        for(auto& modelName : modelPaths){
            if(ImGui::Button(modelName.c_str())){
                // モデルをエミッターに追加
                emitModelFilePath_ = modelName;
            }
        }
        ImGui::Unindent();
    }
}


/*------------------------*/
/*      頻度などの情報      */
/*------------------------*/
void Emitter_Model::EditFrequency(){
    ImGui::DragFloat("発生間隔", &interval, 0.01f, 0.0f);
    ImGui::DragInt("一度に発生する数", &numEmitEvery, 1, 0, 100);
}

///////////////////////////////////////////////////////////
// 描画
///////////////////////////////////////////////////////////
void Emitter_Model::DrawEditData(){

    // アクティブでない場合は描画しない
    if(!isActive){
        return;
    }

    // 範囲の描画
    AABB area;
    area.center = center;
    area.halfSize = emitRange * 0.5f;
    if(parentGroup){
        area.center += parentGroup->GetCenter();
    }

    SEED::DrawAABB(area, { 0.0f,0.0f,1.0f,1.0f });

    // 発生範囲の中心に球を描画
    if(isSetGoalPosition){
        Vector3 goalPos = center + goalPosition;
        SEED::DrawSphere(goalPos, 1.0f, 6,{ 1.0f,0.0f,0.0f,1.0f });
    }
}


///////////////////////////////////////////////////////////
// 出力
///////////////////////////////////////////////////////////
nlohmann::json Emitter_Model::ExportToJson(){
    nlohmann::json j;

    // 全般の情報
    j["emitterType"] = "Emitter_Model3D";
    j["isActive"] = true;
    j["isBillboard"] = isBillboard;
    j["isUseRotate"] = isUseRotate;
    j["isUseGravity"] = isUseGravity;
    j["emitType"] = (int)emitType;
    j["particleType"] = (int)particleType;
    j["blendMode"] = (int)blendMode;
    j["CullingMode"] = (int)cullingMode;
    j["lightingType"] = (int)lightingType_;
    j["center"] = { center.x, center.y, center.z };

    // 範囲・パラメーターなどの情報
    j["emitRange"] = { emitRange.x, emitRange.y, emitRange.z };
    j["radiusRange"] = { radiusRange.min, radiusRange.max };
    j["baseDirection"] = { baseDirection.x, baseDirection.y, baseDirection.z };
    j["directionRange"] = directionRange;
    j["isSetGoalPosition"] = isSetGoalPosition;
    j["isEndWithGoalPosition"] = isEndWithGoalPosition;
    j["goalPosition"] = { goalPosition.x, goalPosition.y, goalPosition.z };
    j["speedRange"] = { speedRange.min, speedRange.max };
    j["rotateSpeedRange"] = { rotateSpeedRange.min, rotateSpeedRange.max };
    j["useRotateDirection"] = useRotateDirection;
    j["isRoteteRandomInit"] = isRoteteRandomInit_;
    j["rotateDirection"] = { rotateDirection.x, rotateDirection.y, rotateDirection.z };
    j["lifeTimeRange"] = { lifeTimeRange.min, lifeTimeRange.max };
    j["gravity"] = gravity;
    j["scaleRange"] = { scaleRange.min, scaleRange.max };

    // 減衰・イージング関数の情報
    j["inScale"] = kInScale;
    j["outScale"] = kOutScale;
    j["inAlpha"] = kInAlpha;
    j["outAlpha"] = kOutAlpha;
    j["maxTimePoint"] = maxTimePoint;
    j["maxTimeRate"] = maxTimeRate;
    j["velocityEaseType"] = (int)velocityEaseType_;
    j["rotateEaseType"] = (int)rotateEaseType_;
    j["enterEaseType"] = (int)enterEaseType_;
    j["exitEaseType"] = (int)exitEaseType_;

    // 発生頻度などの情報
    j["interval"] = interval;
    j["numEmitEvery"] = numEmitEvery;
    j["kMaxEmitCount"] = kMaxEmitCount;

    // 色の情報
    for(auto& color : colors){
        j["colors"].push_back({ color.x, color.y, color.z, color.w });
    }

    // テクスチャの情報
    j["useDefaultTexture"] = useDefaultTexture;
    for(auto& textureHandle : texturePaths){
        j["textureHandles"].push_back(textureHandle);
    }

    // モデルの情報
    j["emitModelFilePath"] = emitModelFilePath_;

    return j;
}

///////////////////////////////////////////////////////////
// 読み込み
///////////////////////////////////////////////////////////
void Emitter_Model::LoadFromJson(const nlohmann::json& j){

    // 全般の情報
    isActive = true;//j["isActive"];
    isBillboard = j["isBillboard"];
    isUseRotate = j["isUseRotate"];
    isUseGravity = j["isUseGravity"];
    emitType = (EmitType)j["emitType"];
    particleType = (ParticleType)j["particleType"];
    blendMode = (BlendMode)j["blendMode"];
    cullingMode = (D3D12_CULL_MODE)j["CullingMode"];
    lightingType_ = (LIGHTING_TYPE)j["lightingType"];
    center = Vector3(
        j["center"][0], j["center"][1], j["center"][2]
    );

    // 範囲やパラメーターなどの情報
    emitRange = Vector3(j["emitRange"]);
    radiusRange = Range1D(j["radiusRange"][0], j["radiusRange"][1]);
    isSetGoalPosition = j["isSetGoalPosition"];
    isEndWithGoalPosition = j["isEndWithGoalPosition"];
    goalPosition = Vector3(j["goalPosition"]);
    baseDirection = Vector3(j["baseDirection"]);
    directionRange = j["directionRange"];
    speedRange = Range1D(j["speedRange"][0], j["speedRange"][1]);
    rotateSpeedRange = Range1D(j["rotateSpeedRange"][0], j["rotateSpeedRange"][1]);
    useRotateDirection = j["useRotateDirection"];
    isRoteteRandomInit_ = j["isRoteteRandomInit"];
    rotateDirection = Vector3(j["rotateDirection"]);
    lifeTimeRange = Range1D(j["lifeTimeRange"][0], j["lifeTimeRange"][1]);
    gravity = j["gravity"];
    scaleRange = Range3D(j["scaleRange"][0], j["scaleRange"][1]);

    // 出現・消失・イージング関数の情報
    kInScale = Vector3(j["inScale"]);
    kOutScale = Vector3(j["outScale"]);
    kInAlpha = j["inAlpha"];
    kOutAlpha = j["outAlpha"];
    maxTimePoint = j["maxTimePoint"];
    maxTimeRate = j["maxTimeRate"];
    velocityEaseType_ = (Easing::Type)j["velocityEaseType"];
    rotateEaseType_ = (Easing::Type)j["rotateEaseType"];
    enterEaseType_ = (Easing::Type)j["enterEaseType"];
    exitEaseType_ = (Easing::Type)j["exitEaseType"];

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
    useDefaultTexture = j["useDefaultTexture"];
    for(auto& textureHandle : j["textureHandles"]){
        texturePaths.push_back(textureHandle);
    }

    // モデルの情報
    emitModelFilePath_ = j["emitModelFilePath"].get<std::string>();
}

