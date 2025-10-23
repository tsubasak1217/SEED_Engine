#include "Emitter3D.h"
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterGroup3D.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>
#include <SEED/Source/Manager/ModelManager/ModelManager.h>
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterBase.h>

Emitter3D::Emitter3D() : EmitterBase(){
    // カーブを初期化
    scaleCurve_ = Curve(CurveChannel::VECTOR3);
    velocityCurve_ = Curve(CurveChannel::FLOAT);
    rotateCurve_ = Curve(CurveChannel::FLOAT);
    colorCurve_ = Curve(CurveChannel::VECTOR4);
}

//
void Emitter3D::Edit(){
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
        Matrix4x4 parentMat = IdentityMat4();
        if(parentGroup_){
            if(auto group3D = dynamic_cast<EmitterGroup3D*>(parentGroup_)){
                parentMat = TranslateMatrix(group3D->GetPosition());
            }
        }

        // 登録
        ImGuiManager::RegisterGuizmoItem(&center, parentMat);
    }

    // 出現範囲の可視化
    AABB rangeBox;
    rangeBox.center = GetCenter();
    rangeBox.halfSize = emitRange * 0.5f;
    SEED::DrawAABB(rangeBox, { 0.0f,1.0f,1.0f,1.0f });

    ImGui::Unindent();
}


/*------------------------*/
/*        全般の情報       */
/*------------------------*/
void Emitter3D::EditGeneral(){
    ImFunc::Combo("発生タイプ" + idTag_, emitType_, { "一度のみ","ずっと","指定回数" });
    if(emitType_ == EmitType::kCustom){
        ImGui::DragInt("発生回数" + idTag_, &kMaxEmitCount_, 1);
    }
}


/*------------------------*/
/*      範囲などの情報      */
/*------------------------*/
void Emitter3D::EditRangeParameters(){
    ImGui::Text("-------- 出現 --------");
    ImGui::DragFloat3("中心座標" + idTag_, &center.translate.x, 0.05f);
    ImGui::DragFloat3("発生範囲" + idTag_, &emitRange.x, 0.05f);

    ImGui::Text("------- 半径 -------");
    ImGui::DragFloat("最小半径" + idTag_, &radiusRange.min, 0.01f, 0.0f, radiusRange.max);
    ImGui::DragFloat("最大半径" + idTag_, &radiusRange.max, 0.01f, radiusRange.min);

    ImGui::Text("------- スケール -------");
    ImGui::DragFloat3("最小倍率(x,y,x)" + idTag_, &scaleRange.min.x, 0.005f, 0.0f, scaleRange.max.x);
    ImGui::DragFloat3("最大倍率(x,y,x)" + idTag_, &scaleRange.max.x, 0.005f, scaleRange.min.x);


    ImGui::Text("------- 移動 -------");
    ImGui::Checkbox("目標位置を設定するか" + idTag_, &isSetGoalPosition_);
    if(isSetGoalPosition_){
        ImGui::DragFloat3("ゴール位置" + idTag_, &goalPosition.x, 0.05f);
    } else{
        if(ImGui::DragFloat3("基礎となる方向" + idTag_, &baseDirection.x, 0.01f)){
            baseDirection = MyMath::Normalize(baseDirection);
        };
    }

    ImGui::DragFloat("方向のばらけ具合" + idTag_, &directionRange, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("最低速度" + idTag_, &speedRange.min, 0.02f, 0.0f, speedRange.max);
    ImGui::DragFloat("最高速度" + idTag_, &speedRange.max, 0.02f, speedRange.min);


    ImGui::Text("------ 寿命 ------");
    ImGui::DragFloat("最短" + idTag_, &lifeTimeRange_.min, 0.05f, 0.0f, lifeTimeRange_.max);
    ImGui::DragFloat("最長" + idTag_, &lifeTimeRange_.max, 0.05f, lifeTimeRange_.min);


    ImGui::Text("------- 回転 -------");
    ImGui::Checkbox("ビルボードするか" + idTag_, &isBillboard);
    ImGui::Checkbox("回転の初期化値をランダムにするか" + idTag_, &isRoteteRandomInit_);
    ImGui::Checkbox("回転するか" + idTag_, &isUseRotate_);
    if(isUseRotate_){
        ImGui::Checkbox("回転軸を指定するか" + idTag_, &useRotateDirection);
        if(useRotateDirection){
            ImGui::DragFloat3("回転軸" + idTag_, &rotateDirection.x, 0.01f);
            rotateDirection = MyMath::Normalize(rotateDirection);
        }
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
void Emitter3D::EditMaterial(){

    // BlendMode, CullingMode,LightingTypeの設定
    ImGui::Text("-------- 描画設定 --------");
    ImFunc::Combo("ブレンドモード" + idTag_, blendMode_, { "NONE","MULTIPLY","SUBTRACT","NORMAL","ADD","SCREEN" });
    ImFunc::Combo("ライティング" + idTag_, lightingType_, { "なし","ランバート","ハーフランバート" });
    ImFunc::Combo("カリング設定" + idTag_, cullingMode_, { "なし","前面","背面" }, 1);

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
        ImGui::Checkbox("モデルのデフォルトテクスチャ使用する" + idTag_, &useDefaultTexture);
        if(!useDefaultTexture){
            static ImVec2 buttonSize = ImVec2(50, 50);
            float curRightX = ImGui::GetCursorPosX();
            float availRegionX = ImGui::GetContentRegionAvail().x;

            if(ImGui::CollapsingHeader("カスタムテクスチャ" + idTag_)){
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
        }
        ImGui::Unindent();
    }

    // モデルの設定
    ImGui::Text("-------- モデル --------");
    if(ImGui::CollapsingHeader("モデル一覧")){
        ImGui::Indent();
        // モデルの一覧から選択したものをエミッターのモデルリストに追加
        static const std::filesystem::path rootPath = "Resources/Models";
        static std::filesystem::path curPath = rootPath;
        std::string selected = ImFunc::FolderView("モデル形状を選択", curPath, false, { ".obj", ".gltf",".glb" }, rootPath);

        // 選択されたら変更
        if(!selected.empty()){
            emitModelFilePath_ = selected;
        }
    }
}


///////////////////////////////////////////////////////////
// 出力
///////////////////////////////////////////////////////////
nlohmann::json Emitter3D::ExportToJson(){
    nlohmann::json j;

    // 全般の情報
    j["emitterType"] = "Emitter3D";
    j["isBillboard"] = isBillboard;
    j["lightingType"] = (int)lightingType_;
    j["center"] = center.translate;

    // 範囲・パラメーターなどの情報
    j["emitRange"] = emitRange;
    j["radiusRange"] = radiusRange;
    j["baseDirection"] = baseDirection;
    j["directionRange"] = directionRange;
    j["goalPosition"] = goalPosition;
    j["speedRange"] = speedRange;
    j["rotateSpeedRange"] = rotateSpeedRange;
    j["useRotateDirection"] = useRotateDirection;
    j["isRoteteRandomInit"] = isRoteteRandomInit_;
    j["rotateDirection"] = rotateDirection;
    j["lifeTimeRange"] = lifeTimeRange_;
    j["gravity"] = gravity_;
    j["scaleRange"] = scaleRange;

    // テクスチャの情報
    j["useDefaultTexture"] = useDefaultTexture;

    // モデルの情報
    j["emitModelFilePath"] = emitModelFilePath_;

    return j;
}

///////////////////////////////////////////////////////////
// 読み込み
///////////////////////////////////////////////////////////
void Emitter3D::LoadFromJson(const nlohmann::json& j){

    // 全般の情報
    isBillboard = j.value("isBillboard", true);
    cullingMode_ = (D3D12_CULL_MODE)j.value("CullingMode", 3);
    lightingType_ = (LIGHTING_TYPE)j.value("lightingType", 0);
    initUpdateTime_ = j.value("initUpdateTime", 0.0f);
    center.translate = j.value("center", Vector3(0.0f));

    // 範囲やパラメーターなどの情報
    emitRange = j.value("emitRange", Vector3());
    radiusRange = j.value("radiusRange", Range1D());
    goalPosition = j.value("goalPosition", Vector3());
    baseDirection = j.value("baseDirection", Vector3(0.0f, 1.0f, 0.0f));
    directionRange = j.value("directionRange", 0.3f);
    speedRange = j.value("speedRange", Range1D());
    rotateSpeedRange = j.value("rotateSpeedRange", Range1D());
    useRotateDirection = j.value("useRotateDirection", false);
    isRoteteRandomInit_ = j.value("isRoteteRandomInit", false);
    rotateDirection = j.value("rotateDirection", Vector3());
    lifeTimeRange_ = j.value("lifeTimeRange", Range1D());
    scaleRange = j.value("scaleRange", Range3D());

    // モデルの情報
    emitModelFilePath_ = j.value("emitModelFilePath", "DefaultAssets/Plane/Plane.obj");
}


// 発生位置の中心を取得
Vector3 Emitter3D::GetCenter() const{
    if(parentGroup_){
        if(EmitterGroup3D* group3D = dynamic_cast<EmitterGroup3D*>(parentGroup_)){
            return center.translate + group3D->GetPosition();
        }
    }
    return center.translate;
}