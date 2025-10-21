#include "Emitter2D.h"
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterGroup2D.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>
#include <SEED/Source/Manager/ModelManager/ModelManager.h>
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterBase.h>

Emitter2D::Emitter2D() : EmitterBase(){
    // カーブを初期化
    scaleCurve_ = Curve(CurveChannel::VECTOR2);
    velocityCurve_ = Curve(CurveChannel::FLOAT);
    rotateCurve_ = Curve(CurveChannel::FLOAT);
    colorCurve_ = Curve(CurveChannel::VECTOR4);
}

//
void Emitter2D::Edit(){
    ImGui::Indent();

    // Guizmoの表示フラグ
    ImGui::Checkbox("Guizmoを表示" + idTag_, &useGuizmo_);

    // 全般の情報
    if(ImGui::CollapsingHeader("全般" + idTag_)){
        ImGui::Indent();
        EditGeneral();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }


    // 範囲などの情報
    if(ImGui::CollapsingHeader("範囲 / パラメーター" + idTag_)){
        ImGui::Indent();
        EditRangeParameters();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }

    // イージング関数の情報
    if(ImGui::CollapsingHeader("出現/消滅パラメーター・イージング" + idTag_)){
        ImGui::Indent();
        EditEaseType();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }

    // マテリアルなどの情報
    if(ImGui::CollapsingHeader("マテリアル・形状・描画設定" + idTag_)){
        ImGui::Indent();
        EditMaterial();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }


    // 頻度などの情報
    if(ImGui::CollapsingHeader("発生頻度・個数" + idTag_)){
        ImGui::Indent();
        EditFrequency();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }

    // Guizmoの登録
    if(useGuizmo_){
        // 親グループの位置を考慮する
        Matrix3x3 parentMat = IdentityMat3();
        if(parentGroup){
            if(auto group2D = dynamic_cast<EmitterGroup2D*>(parentGroup)){
                parentMat = TranslateMatrix(group2D->GetPosition());
            }
        }

        // 登録
        ImGuiManager::RegisterGuizmoItem(&center, parentMat.ToMat4x4());
    }

    // 出現範囲の可視化
    AABB2D rangeBox;
    rangeBox.center = GetCenter();
    rangeBox.halfSize = emitRange * 0.5f;
    SEED::DrawAABB2D(rangeBox, { 0.0f,1.0f,1.0f,1.0f });

    ImGui::Unindent();
}


/*------------------------*/
/*        全般の情報       */
/*------------------------*/
void Emitter2D::EditGeneral(){
    ImFunc::Combo("発生タイプ" + idTag_, emitType, { "一度のみ","ずっと","指定回数" });
    if(emitType == EmitType::kCustom){
        ImGui::DragInt("発生回数" + idTag_, &kMaxEmitCount, 1);
    }
}


/*------------------------*/
/*      範囲などの情報      */
/*------------------------*/
void Emitter2D::EditRangeParameters(){
    ImGui::Text("-------- 出現 --------");
    ImGui::DragFloat2("中心座標" + idTag_, &center.translate.x, 0.05f);
    ImGui::DragFloat2("発生範囲" + idTag_, &emitRange.x, 0.05f);

    ImGui::Text("------- 半径 -------");
    ImGui::DragFloat("最小半径" + idTag_, &radiusRange.min, 0.01f, 0.0f, radiusRange.max);
    ImGui::DragFloat("最大半径" + idTag_, &radiusRange.max, 0.01f, radiusRange.min);

    ImGui::Text("------- スケール -------");
    ImGui::DragFloat2("最小倍率(x,y)" + idTag_, &scaleRange.min.x, 0.005f, 0.0f, scaleRange.max.x);
    ImGui::DragFloat2("最大倍率(x,y)" + idTag_, &scaleRange.max.x, 0.005f, scaleRange.min.x);


    ImGui::Text("------- 移動 -------");
    ImGui::Checkbox("目標位置を設定するか" + idTag_, &isSetGoalPosition);
    if(isSetGoalPosition){
        ImGui::Checkbox("目標位置で終了するか" + idTag_, &isEndWithGoalPosition);
        ImGui::DragFloat2("ゴール位置" + idTag_, &goalPosition.x, 0.05f);
    } else{
        isEndWithGoalPosition = false;
        if(ImGui::DragFloat2("基礎となる方向" + idTag_, &baseDirection.x, 0.01f)){
            baseDirection = MyMath::Normalize(baseDirection);
        };
    }
    if(!isEndWithGoalPosition){
        ImGui::DragFloat("方向のばらけ具合" + idTag_, &directionRange, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("最低速度" + idTag_, &speedRange.min, 0.02f, 0.0f, speedRange.max);
        ImGui::DragFloat("最高速度" + idTag_, &speedRange.max, 0.02f, speedRange.min);
    }


    ImGui::Text("------ 寿命 ------");
    ImGui::DragFloat("最短" + idTag_, &lifeTimeRange.min, 0.05f, 0.0f, lifeTimeRange.max);
    ImGui::DragFloat("最長" + idTag_, &lifeTimeRange.max, 0.05f, lifeTimeRange.min);


    ImGui::Text("------- 回転 -------");
    ImGui::Checkbox("回転の初期化値をランダムにするか" + idTag_, &isRoteteRandomInit_);
    ImGui::Checkbox("回転するか" + idTag_, &isUseRotate);
    if(isUseRotate){
        ImGui::DragFloat("最小回転速度" + idTag_, &rotateSpeedRange.min, 0.01f, -10000.0f, rotateSpeedRange.max);
        ImGui::DragFloat("最大回転速度" + idTag_, &rotateSpeedRange.max, 0.01f, rotateSpeedRange.min);
    }


    ImGui::Text("------- 重力 -------");
    ImGui::Checkbox("重力を使用するか" + idTag_, &isUseGravity);
    if(isUseGravity){
        ImGui::DragFloat("重力" + idTag_, &gravity, 0.01f);
    }
}


/*------------------------*/
/*      イージング関数の情報  */
/*------------------------*/
void Emitter2D::EditEaseType(){
    ImGui::Checkbox("カーブを使用するか" + idTag_, &useCurve_);

    // カーブを使用しない場合はイージング関数で設定
    if(!useCurve_){
        ImGui::Text("-------- 出現・消失パラメーター --------");
        ImGui::DragFloat2("出現時のスケール" + idTag_, &kInScale.x, 0.01f);
        ImGui::DragFloat2("消失時のスケール" + idTag_, &kOutScale.x, 0.01f);
        ImGui::SliderFloat("出現時のアルファ値" + idTag_, &kInAlpha, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("消失時のアルファ値" + idTag_, &kOutAlpha, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("最大に到達する時間" + idTag_, &maxTimePoint, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("最大を維持する時間割合" + idTag_, &maxTimeRate, 0.0f, 1.0f, "%.2f");
        ImGui::Text("-------- イージング関数 --------");
        ImFunc::Combo("速度", velocityEaseType_, Easing::names, IM_ARRAYSIZE(Easing::names));
        ImFunc::Combo("回転", rotateEaseType_, Easing::names, IM_ARRAYSIZE(Easing::names));
        ImFunc::Combo("出現", enterEaseType_, Easing::names, IM_ARRAYSIZE(Easing::names));
        ImFunc::Combo("消滅", exitEaseType_, Easing::names, IM_ARRAYSIZE(Easing::names));
    } else{
        ImGui::Text("-------- カーブ編集 --------");
        EditCurves();
    }
}


/*------------------------*/
/*     マテリアルなどの情報  */
/*------------------------*/
void Emitter2D::EditMaterial(){

    // BlendMode, CullingMode,LightingTypeの設定
    ImGui::Text("-------- 描画設定 --------");
    ImFunc::Combo("ブレンドモード" + idTag_, blendMode, { "NONE","MULTIPLY","SUBTRACT","NORMAL","ADD","SCREEN" });

    // 色の設定
    ImGui::Text("-------- 色 --------");
    if(ImGui::CollapsingHeader("出現色の一覧(マスターカラー)" + idTag_)){
        ImGui::Indent();

        // 色のリスト
        for(int32_t i = 0; i < (int)colors.size(); ++i){
            std::string colorLabel = "##" + MyFunc::PtrToStr(&colors[i]);
            ImGui::ColorEdit4("color" + colorLabel, &colors[i].value.x);
            // 削除ボタン
            if(ImGui::Button("削除" + colorLabel)){
                if(colors.size() > 1){
                    colors.erase(colors.begin() + i);
                    break;
                }
            }
        }

        // 追加ボタン
        ImGui::Text("-- 色の追加 --");
        if(ImGui::Button("追加" + idTag_)){
            colors.emplace_back(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
        }

        ImGui::Unindent();
    }

    // テクスチャの設定
    ImGui::Text("-------- テクスチャ --------");
    if(ImGui::CollapsingHeader("テクスチャ設定" + idTag_)){
        ImGui::Indent();

        // エミッターに追加されたテクスチャのリスト
        static ImVec2 buttonSize = ImVec2(50, 50);
        float curRightX = ImGui::GetCursorPosX();
        float availRegionX = ImGui::GetContentRegionAvail().x;

        if(ImGui::CollapsingHeader("出現テクスチャ一覧" + idTag_)){
            ImGui::Indent();

            // 画像の一覧から選択したものをエミッターのテクスチャリストに追加
            for(int32_t i = 0; i < texturePaths.size(); i++){

                // テクスチャの表示(押したら消す)
                if(ImGui::ImageButton(MyFunc::PtrToStr(&texturePaths[i]), textureDict[texturePaths[i]], buttonSize)){
                    // 消す
                    if(texturePaths.size() > 1){
                        texturePaths.erase(
                            std::remove(texturePaths.begin(), texturePaths.end(), texturePaths[i]),
                            texturePaths.end()
                        );
                    }
                }

                // ボタンの位置を更新
                ImGui::SameLine();
                curRightX = ImGui::GetCursorPosX();

                // 右端まで来ていたら改行
                if(curRightX > availRegionX){
                    ImGui::NewLine();
                    curRightX = 0.0f;
                }
            }

            ImGui::NewLine();
            ImGui::Unindent();
        }

        // テクスチャの追加用のフォルダビュー
        static const std::filesystem::path rootPath = "Resources/Textures";
        static std::filesystem::path curPath = rootPath;
        std::string selected = ImFunc::FolderView("追加テクスチャを選択", curPath, false, { ".png", ".jpg" }, rootPath);

        // テクスチャが選択されたら追加
        if(!selected.empty()){
            // テクスチャをエミッターに追加
            textureDict[selected] = TextureManager::GetImGuiTexture(selected);
            texturePaths.push_back(selected);
        }

        ImGui::Unindent();
    }
}


/*------------------------*/
/*      頻度などの情報      */
/*------------------------*/
void Emitter2D::EditFrequency(){
    ImGui::DragFloat("発生間隔", &interval, 0.01f, 0.0f);
    ImGui::DragInt("一度に発生する数", &numEmitEvery, 1, 0, 100);
}


///////////////////////////////////////////////////////////
// 出力
///////////////////////////////////////////////////////////
nlohmann::json Emitter2D::ExportToJson(){
    nlohmann::json j;

    // 全般の情報
    j["emitterType"] = "Emitter2D";
    j["isActive"] = true;
    j["isUseRotate"] = isUseRotate;
    j["isUseGravity"] = isUseGravity;
    j["emitType"] = (int)emitType;
    j["blendMode"] = (int)blendMode;
    j["initUpdateTime"] = initUpdateTime_;
    j["center"] = center.translate;

    // 範囲・パラメーターなどの情報
    j["emitRange"] = { emitRange.x, emitRange.y };
    j["radiusRange"] = { radiusRange.min, radiusRange.max };
    j["baseDirection"] = { baseDirection.x, baseDirection.y };
    j["directionRange"] = directionRange;
    j["isSetGoalPosition"] = isSetGoalPosition;
    j["isEndWithGoalPosition"] = isEndWithGoalPosition;
    j["goalPosition"] = goalPosition;
    j["speedRange"] = { speedRange.min, speedRange.max };
    j["rotateSpeedRange"] = { rotateSpeedRange.min, rotateSpeedRange.max };
    j["isRoteteRandomInit"] = isRoteteRandomInit_;
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

    // カーブの情報
    j["useCurve"] = useCurve_;
    if(useCurve_){
        j["scaleCurve"] = scaleCurve_.ToJson();
        j["velocityCurve"] = velocityCurve_.ToJson();
        j["rotateCurve"] = rotateCurve_.ToJson();
        j["colorCurve"] = colorCurve_.ToJson();
        j["colorMode"] = (int)colorMode_;
    }

    // 発生頻度などの情報
    j["interval"] = interval;
    j["numEmitEvery"] = numEmitEvery;
    j["kMaxEmitCount"] = kMaxEmitCount;

    // 色の情報
    for(auto& color : colors){
        j["colors"].push_back({ color.value.x, color.value.y, color.value.z, color.value.w });
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
void Emitter2D::LoadFromJson(const nlohmann::json& j){

    // 全般の情報
    isActive = true;//j["isActive"];
    isUseRotate = j["isUseRotate"];
    isUseGravity = j["isUseGravity"];
    emitType = (EmitType)j["emitType"];
    blendMode = (BlendMode)j["blendMode"];
    initUpdateTime_ = j.value("initUpdateTime", 0.0f);
    center.translate = j.value("center", Vector2(0.0f));

    // 範囲やパラメーターなどの情報
    emitRange = j["emitRange"];
    radiusRange = Range1D(j["radiusRange"][0], j["radiusRange"][1]);
    isSetGoalPosition = j["isSetGoalPosition"];
    isEndWithGoalPosition = j["isEndWithGoalPosition"];
    goalPosition = j["goalPosition"];
    baseDirection = j["baseDirection"];
    directionRange = j["directionRange"];
    speedRange = Range1D(j["speedRange"][0], j["speedRange"][1]);
    rotateSpeedRange = Range1D(j["rotateSpeedRange"][0], j["rotateSpeedRange"][1]);
    isRoteteRandomInit_ = j["isRoteteRandomInit"];
    lifeTimeRange = Range1D(j["lifeTimeRange"][0], j["lifeTimeRange"][1]);
    gravity = j["gravity"];
    scaleRange = Range2D(j["scaleRange"][0], j["scaleRange"][1]);

    // 出現・消失・イージング関数の情報
    kInScale = j["inScale"];
    kOutScale = j["outScale"];
    kInAlpha = j["inAlpha"];
    kOutAlpha = j["outAlpha"];
    maxTimePoint = j["maxTimePoint"];
    maxTimeRate = j["maxTimeRate"];
    velocityEaseType_ = (Easing::Type)j["velocityEaseType"];
    rotateEaseType_ = (Easing::Type)j["rotateEaseType"];
    enterEaseType_ = (Easing::Type)j["enterEaseType"];
    exitEaseType_ = (Easing::Type)j["exitEaseType"];

    // カーブの情報
    useCurve_ = j["useCurve"];
    if(useCurve_){
        scaleCurve_.FromJson(j["scaleCurve"]);
        velocityCurve_.FromJson(j["velocityCurve"]);
        rotateCurve_.FromJson(j["rotateCurve"]);
        colorCurve_.FromJson(j["colorCurve"]);
        colorMode_ = (ColorMode)j["colorMode"];
    }

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
        // テクスチャのセットに追加
        textureDict[textureHandle] = TextureManager::GetImGuiTexture(textureHandle);
    }
}


// 発生位置の中心を取得
Vector2 Emitter2D::GetCenter() const{
    if(parentGroup){
        if(EmitterGroup2D* group2D = dynamic_cast<EmitterGroup2D*>(parentGroup)){
            return center.translate + group2D->GetPosition();
        }
    }
    return center.translate;
}