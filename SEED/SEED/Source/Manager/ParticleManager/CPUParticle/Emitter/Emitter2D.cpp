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

#ifdef _DEBUG
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
    if(ImGui::CollapsingHeader("カーブ編集" + idTag_)){
        ImGui::Indent();
        EditCurves();
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
        if(parentGroup_){
            if(auto group2D = dynamic_cast<EmitterGroup2D*>(parentGroup_)){
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
    ImGui::Checkbox("目標位置を設定するか" + idTag_, &isSetGoalPosition_);
    if(isSetGoalPosition_){
        ImGui::DragFloat2("ゴール位置" + idTag_, &goalPosition.x, 0.05f);
    } else{
        if(ImGui::DragFloat2("基礎となる方向" + idTag_, &baseDirection.x, 0.01f)){
            baseDirection = MyMath::Normalize(baseDirection);
        };
    }

    ImGui::DragFloat("方向のばらけ具合" + idTag_, &directionRange, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("最低速度" + idTag_, &speedRange.min, 0.02f, 0.0f, speedRange.max);
    ImGui::DragFloat("最高速度" + idTag_, &speedRange.max, 0.02f, speedRange.min);


    ImGui::Text("------ 寿命 ------");
    ImGui::DragFloat("最短" + idTag_, &lifeTimeRange.min, 0.05f, 0.0f, lifeTimeRange.max);
    ImGui::DragFloat("最長" + idTag_, &lifeTimeRange.max, 0.05f, lifeTimeRange.min);


    ImGui::Text("------- 回転 -------");
    ImGui::Checkbox("回転の初期化値をランダムにするか" + idTag_, &isRoteteRandomInit_);
    ImGui::Checkbox("回転するか" + idTag_, &isUseRotate_);
    if(isUseRotate_){
        ImGui::DragFloat("最小回転速度" + idTag_, &rotateSpeedRange.min, 0.01f, -10000.0f, rotateSpeedRange.max);
        ImGui::DragFloat("最大回転速度" + idTag_, &rotateSpeedRange.max, 0.01f, rotateSpeedRange.min);
    }


    ImGui::Text("------- 重力 -------");
    ImGui::Checkbox("重力を使用するか" + idTag_, &isUseGravity_);
    if(isUseGravity_){
        ImGui::DragFloat("重力" + idTag_, &gravity_, 0.01f);
    }
}

/*------------------------*/
/*     マテリアルなどの情報  */
/*------------------------*/
void Emitter2D::EditMaterial(){

    // BlendMode, CullingMode,LightingTypeの設定
    ImGui::Text("-------- 描画設定 --------");
    ImFunc::Combo("ブレンドモード" + idTag_, blendMode_, { "NONE","MULTIPLY","SUBTRACT","NORMAL","ADD","SCREEN" });

    // 色の設定
    ImGui::Text("-------- 色 --------");
    if(ImGui::CollapsingHeader("出現色の一覧(マスターカラー)" + idTag_)){
        ImGui::Indent();

        // 色のリスト
        for(int32_t i = 0; i < (int)colors_.size(); ++i){
            std::string colorLabel = "##" + MyFunc::PtrToStr(&colors_[i]);
            ImGui::ColorEdit4("color" + colorLabel, &colors_[i].value.x);
            // 削除ボタン
            if(ImGui::Button("削除" + colorLabel)){
                if(colors_.size() > 1){
                    colors_.erase(colors_.begin() + i);
                    break;
                }
            }
        }

        // 追加ボタン
        ImGui::Text("-- 色の追加 --");
        if(ImGui::Button("追加" + idTag_)){
            colors_.emplace_back(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
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
            for(int32_t i = 0; i < texturePaths_.size(); i++){

                // テクスチャの表示(押したら消す)
                if(ImGui::ImageButton(MyFunc::PtrToStr(&texturePaths_[i]), textureDict_[texturePaths_[i]], buttonSize)){
                    // 消す
                    if(texturePaths_.size() > 1){
                        texturePaths_.erase(
                            std::remove(texturePaths_.begin(), texturePaths_.end(), texturePaths_[i]),
                            texturePaths_.end()
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
            textureDict_[selected] = TextureManager::GetImGuiTexture(selected);
            texturePaths_.push_back(selected);
        }

        ImGui::Unindent();
    }
}

#endif // _DEBUG


///////////////////////////////////////////////////////////
// 出力
///////////////////////////////////////////////////////////
nlohmann::json Emitter2D::ExportToJson(){
    nlohmann::json j;

    // 全般の情報
    j["emitterType"] = "Emitter2D";
    j["isActive"] = true;
    j["isUseRotate"] = isUseRotate_;
    j["isUseGravity"] = isUseGravity_;
    j["emitType"] = (int)emitType_;
    j["blendMode"] = (int)blendMode_;
    j["initUpdateTime"] = initUpdateTime_;
    j["center"] = center.translate;

    // 範囲・パラメーターなどの情報
    j["emitRange"] = { emitRange.x, emitRange.y };
    j["radiusRange"] = { radiusRange.min, radiusRange.max };
    j["baseDirection"] = { baseDirection.x, baseDirection.y };
    j["directionRange"] = directionRange;
    j["isSetGoalPosition"] = isSetGoalPosition_;
    j["goalPosition"] = goalPosition;
    j["speedRange"] = { speedRange.min, speedRange.max };
    j["rotateSpeedRange"] = { rotateSpeedRange.min, rotateSpeedRange.max };
    j["isRoteteRandomInit"] = isRoteteRandomInit_;
    j["lifeTimeRange"] = { lifeTimeRange.min, lifeTimeRange.max };
    j["gravity"] = gravity_;
    j["scaleRange"] = { scaleRange.min, scaleRange.max };

    // カーブの情報
    j["scaleCurve"] = scaleCurve_.ToJson();
    j["velocityCurve"] = velocityCurve_.ToJson();
    j["rotateCurve"] = rotateCurve_.ToJson();
    j["colorCurve"] = colorCurve_.ToJson();
    j["colorMode"] = (int)colorMode_;

    // 発生頻度などの情報
    j["interval"] = interval_;
    j["numEmitEvery"] = numEmitEvery_;
    j["kMaxEmitCount"] = kMaxEmitCount_;

    // 色の情報
    for(auto& color : colors_){
        j["colors"].push_back({ color.value.x, color.value.y, color.value.z, color.value.w });
    }

    // テクスチャの情報
    for(auto& textureHandle : texturePaths_){
        j["textureHandles"].push_back(textureHandle);
    }

    return j;
}

///////////////////////////////////////////////////////////
// 読み込み
///////////////////////////////////////////////////////////
void Emitter2D::LoadFromJson(const nlohmann::json& j){

    // 全般の情報
    isUseRotate_ = j["isUseRotate"];
    isUseGravity_ = j["isUseGravity"];
    emitType_ = (EmitType)j["emitType"];
    blendMode_ = (BlendMode)j["blendMode"];
    initUpdateTime_ = j.value("initUpdateTime", 0.0f);
    center.translate = j.value("center", Vector2(0.0f));

    // 範囲やパラメーターなどの情報
    emitRange = j["emitRange"];
    radiusRange = Range1D(j["radiusRange"][0], j["radiusRange"][1]);
    isSetGoalPosition_ = j["isSetGoalPosition"];
    goalPosition = j["goalPosition"];
    baseDirection = j["baseDirection"];
    directionRange = j["directionRange"];
    speedRange = Range1D(j["speedRange"][0], j["speedRange"][1]);
    rotateSpeedRange = Range1D(j["rotateSpeedRange"][0], j["rotateSpeedRange"][1]);
    isRoteteRandomInit_ = j["isRoteteRandomInit"];
    lifeTimeRange = Range1D(j["lifeTimeRange"][0], j["lifeTimeRange"][1]);
    gravity_ = j["gravity"];
    scaleRange = Range2D(j["scaleRange"][0], j["scaleRange"][1]);

    // カーブの情報
    scaleCurve_.FromJson(j["scaleCurve"]);
    velocityCurve_.FromJson(j["velocityCurve"]);
    rotateCurve_.FromJson(j["rotateCurve"]);
    colorCurve_.FromJson(j["colorCurve"]);
    colorMode_ = (ColorMode)j["colorMode"];

    // 発生頻度などの情報
    interval_ = j["interval"];
    numEmitEvery_ = j["numEmitEvery"];
    kMaxEmitCount_ = j["kMaxEmitCount"];

    // 色の情報
    colors_.clear();
    for(auto& color : j["colors"]){
        colors_.emplace_back(Vector4(color[0], color[1], color[2], color[3]));
    }

    // テクスチャの情報
    texturePaths_.clear();
    for(auto& textureHandle : j["textureHandles"]){
        texturePaths_.push_back(textureHandle);
        // テクスチャのセットに追加
        textureDict_[textureHandle] = TextureManager::GetImGuiTexture(textureHandle);
    }
}


// 発生位置の中心を取得
Vector2 Emitter2D::GetCenter() const{
    if(parentGroup_){
        if(EmitterGroup2D* group2D = dynamic_cast<EmitterGroup2D*>(parentGroup_)){
            return center.translate + group2D->GetPosition();
        }
    }
    return center.translate;
}