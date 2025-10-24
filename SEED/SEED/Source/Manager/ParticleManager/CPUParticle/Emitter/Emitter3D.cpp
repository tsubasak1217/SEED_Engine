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
    positionInterpolationCurve_ = Curve(CurveChannel::FLOAT);
}


#ifdef _DEBUG
void Emitter3D::Edit(){

    ImGui::Indent();

    // Guizmoの表示フラグ
    ImGui::Checkbox("Guizmoを表示" + idTag_, &useGuizmo_);

    // 全般の情報
    if(ImFunc::CollapsingHeader("全般" + idTag_)){
        ImGui::Indent();
        EditGeneral();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }

    // 頻度などの情報
    if(ImFunc::CollapsingHeader("頻度・寿命" + idTag_)){
        ImGui::Indent();
        EditFrequency();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }

    // 範囲などの情報
    if(ImFunc::CollapsingHeader("トランスフォーム設定" + idTag_)){
        ImGui::Indent();
        EditTransformSettings();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }

    // 色の情報
    if(ImFunc::CollapsingHeader("色設定" + idTag_)){
        ImGui::Indent();
        EditColors();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }

    // マテリアルなどの情報
    if(ImFunc::CollapsingHeader("テクスチャ・描画設定" + idTag_)){
        ImGui::Indent();
        EditTextureAndDrawSettings();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }

    // モデルの情報
    if(ImFunc::CollapsingHeader("モデル設定" + idTag_)){
        ImGui::Indent();
        EditModel();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
    }


    // Guizmoの登録
    if(useGuizmo_){

        // スケールが1でない場合に範囲をスケーリングする
        if(Input::IsPressMouse(MOUSE_BUTTON::LEFT)){
            static Vector3 prevRange;
            if(Input::IsTriggerMouse(MOUSE_BUTTON::LEFT)){
                prevRange = emitRange_;
            }
            if(center_.scale != Vector3(1.0f)){
                emitRange_ = prevRange * center_.scale;
            }
        } else{
            center_.scale = Vector3(1.0f);
        }

        // 親グループの位置を考慮する
        Matrix4x4 parentMat = IdentityMat4();
        if(parentGroup_){
            if(auto group3D = dynamic_cast<EmitterGroup3D*>(parentGroup_)){
                parentMat = TranslateMatrix(group3D->GetPosition());
            }
        }

        // 登録
        ImGuiManager::RegisterGuizmoItem(&center_, parentMat);
    }

    // 出現範囲の可視化
    AABB rangeBox;
    rangeBox.center = GetCenter();
    rangeBox.halfSize = emitRange_ * 0.5f;
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
void Emitter3D::EditTransformSettings(){

    // 座標関連------------------------------------------------
    if(ImFunc::CollapsingHeader("エミッター座標・範囲" + idTag_)){
        ImGui::Indent();
        ImGui::DragFloat3("エミッター中心座標" + idTag_, &center_.translate.x, 0.05f);
        ImGui::DragFloat3("発生範囲" + idTag_, &emitRange_.x, 0.05f);
        ImGui::Unindent();
    }

    // 大きさ関連の情報-----------------------------------------
    if(ImFunc::CollapsingHeader("大きさ" + idTag_)){
        ImGui::Indent();
        ImGui::SeparatorText("半径");
        ImGui::DragFloat("最小半径" + idTag_, &radiusRange_.min, 0.01f, 0.0f, radiusRange_.max);
        ImGui::DragFloat("最大半径" + idTag_, &radiusRange_.max, 0.01f, radiusRange_.min, FLT_MAX);

        ImGui::SeparatorText("スケール");
        ImGui::DragFloat3("最小倍率(x,y,x)" + idTag_, &scaleRange_.min.x, 0.005f, 0.0f, scaleRange_.max.x);
        ImGui::DragFloat3("最大倍率(x,y,x)" + idTag_, &scaleRange_.max.x, 0.005f, scaleRange_.min.x, FLT_MAX);

        // スケールカーブ編集
        if(ImGui::CollapsingHeader("スケールカーブ" + idTag_)){
            ImGui::Indent();
            scaleCurve_.Edit();
            ImGui::Unindent();
        }
        ImGui::Unindent();
    }

    // 移動関連の情報-------------------------------------------
    if(ImFunc::CollapsingHeader("移動" + idTag_)){
        ImGui::Indent();
        if(ImGui::DragFloat3("基礎となる発射方向" + idTag_, &baseDirection_.x, 0.01f)){
            baseDirection_ = MyMath::Normalize(baseDirection_);
        };

        // 基本の方向からのばらけ具合
        ImGui::DragFloat("方向のばらけ具合" + idTag_, &directionRange_, 0.01f, 0.0f, 1.0f);
        ImFunc::HelpTip("0に近いほど基礎方向に揃い、1に近いほどばらける");
        ImGui::DragFloat("最低速度" + idTag_, &speedRange_.min, 0.02f, 0.0f, speedRange_.max);
        ImGui::DragFloat("最高速度" + idTag_, &speedRange_.max, 0.02f, speedRange_.min, FLT_MAX);

        // 重力関連の設定
        ImGui::Checkbox("重力を使用するか" + idTag_, &isUseGravity_);
        if(isUseGravity_){
            ImGui::DragFloat("重力加速度" + idTag_, &gravity_, 0.01f);
        }

        // 終了時に目標位置に移動するかどうか
        ImGui::Checkbox("目標位置を設定するか" + idTag_, &isSetGoalPosition_);
        ImFunc::HelpTip("目標位置を設定すると、パーティクルは寿命終了時にその位置に移動します");
        if(isSetGoalPosition_){
            ImGui::DragFloat3("ゴール位置" + idTag_, &goalPosition_.x, 0.05f);

            // 移動速度のカーブ編集
            if(ImGui::CollapsingHeader("目標点までの補間カーブ" + idTag_)){
                ImGui::Indent();
                positionInterpolationCurve_.Edit();
                ImGui::Unindent();
            }

            // ゴール位置の可視化
            Vector3 pos = GetCenter() + goalPosition_;
            SEED::DrawSphere(pos, 1.0f, 6, { 1.0f,0.0f,0.0f,1.0f });
        }


        // 移動速度のカーブ編集
        if(ImGui::CollapsingHeader("移動速度カーブ" + idTag_)){
            ImGui::Indent();
            velocityCurve_.Edit();
            ImGui::Unindent();
        }

        ImGui::Unindent();
    }


    // 回転関連の情報-------------------------------------------
    if(ImFunc::CollapsingHeader("回転" + idTag_)){
        ImGui::Indent();
        ImGui::Checkbox("ビルボードするか" + idTag_, &isBillboard_);
        ImGui::Checkbox("回転の初期化値をランダムにするか" + idTag_, &isRoteteRandomInit_);
        ImGui::Checkbox("回転するか" + idTag_, &isUseRotate_);
        if(isUseRotate_){
            // 回転軸の設定
            ImGui::Checkbox("回転軸を指定するか" + idTag_, &useRotateDirection_);
            if(useRotateDirection_){
                ImGui::DragFloat3("回転軸" + idTag_, &rotateDirection_.x, 0.01f);
                rotateDirection_ = MyMath::Normalize(rotateDirection_);
            }
            // 回転速度範囲の設定
            ImGui::DragFloat("最小回転速度" + idTag_, &rotateSpeedRange_.min, 0.01f, -FLT_MAX, rotateSpeedRange_.max);
            ImGui::DragFloat("最大回転速度" + idTag_, &rotateSpeedRange_.max, 0.01f, rotateSpeedRange_.min, FLT_MAX);

            // 回転速度のカーブ編集
            if(ImGui::CollapsingHeader("回転速度カーブ" + idTag_)){
                ImGui::Indent();
                rotateCurve_.Edit();
                ImGui::Unindent();
            }
        }
        ImGui::Unindent();
    }
}


/*------------------------*/
/*     マテリアルなどの情報  */
/*------------------------*/
void Emitter3D::EditTextureAndDrawSettings(){

    // BlendMode, CullingMode,LightingTypeの設定
    ImGui::Text("-------- 描画設定 --------");
    ImFunc::Combo("ブレンドモード" + idTag_, blendMode_, { "NONE","MULTIPLY","SUBTRACT","NORMAL","ADD","SCREEN" });
    ImFunc::Combo("ライティング" + idTag_, lightingType_, { "なし","ランバート","ハーフランバート" });
    ImFunc::Combo("カリング設定" + idTag_, cullingMode_, { "なし","前面","背面" }, 1);


    // テクスチャの設定
    ImGui::Text("-------- テクスチャ --------");
    if(ImGui::CollapsingHeader("テクスチャ設定" + idTag_)){
        ImGui::Indent();

        // エミッターに追加されたテクスチャのリスト
        ImGui::Checkbox("モデルのデフォルトテクスチャ使用する" + idTag_, &useDefaultTexture_);
        if(!useDefaultTexture_){
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
}

/*------------------------*/
/*       モデルの情報        */
/*------------------------*/
void Emitter3D::EditModel(){

    // モデルの一覧から選択したものをエミッターのモデルリストに追加
    static const std::filesystem::path rootPath = "Resources/Models";
    static std::filesystem::path curPath = rootPath;
    std::string selected = ImFunc::FolderView("モデル形状を選択", curPath, false, { ".obj", ".gltf",".glb" }, rootPath);

    // 選択されたら変更
    if(!selected.empty()){
        emitModelFilePath_ = selected;
    }
}

#endif // _DEBUG

///////////////////////////////////////////////////////////
// 出力
///////////////////////////////////////////////////////////
nlohmann::json Emitter3D::ExportToJson(){
    nlohmann::json j;

    // 基底クラスの情報を書き込む
    j.update(EmitterBase::ExportToJson());

    // 全般の情報
    j["emitterType"] = "Emitter3D";
    j["isBillboard"] = isBillboard_;
    j["lightingType"] = (int)lightingType_;
    j["center"] = center_.translate;

    // 範囲・パラメーターなどの情報
    j["emitRange"] = emitRange_;
    j["radiusRange"] = radiusRange_;
    j["baseDirection"] = baseDirection_;
    j["directionRange"] = directionRange_;
    j["goalPosition"] = goalPosition_;
    j["speedRange"] = speedRange_;
    j["rotateSpeedRange"] = rotateSpeedRange_;
    j["useRotateDirection"] = useRotateDirection_;
    j["rotateDirection"] = rotateDirection_;
    j["lifeTimeRange"] = lifeTimeRange_;
    j["gravity"] = gravity_;
    j["scaleRange"] = scaleRange_;

    // テクスチャの情報
    j["useDefaultTexture"] = useDefaultTexture_;

    // モデルの情報
    j["emitModelFilePath"] = emitModelFilePath_;

    return j;
}

///////////////////////////////////////////////////////////
// 読み込み
///////////////////////////////////////////////////////////
void Emitter3D::LoadFromJson(const nlohmann::json& j){

    // 基底クラスの情報を読み込む
    EmitterBase::LoadFromJson(j);

    // 全般の情報
    isBillboard_ = j.value("isBillboard", true);
    cullingMode_ = (D3D12_CULL_MODE)j.value("CullingMode", 3);
    lightingType_ = (LIGHTING_TYPE)j.value("lightingType", 0);
    center_.translate = j.value("center", Vector3(0.0f));

    // 範囲やパラメーターなどの情報
    emitRange_ = j.value("emitRange", Vector3());
    radiusRange_ = j.value("radiusRange", Range1D());
    goalPosition_ = j.value("goalPosition", Vector3());
    baseDirection_ = j.value("baseDirection", Vector3(0.0f, 1.0f, 0.0f));
    directionRange_ = j.value("directionRange", 0.3f);
    speedRange_ = j.value("speedRange", Range1D());
    rotateSpeedRange_ = j.value("rotateSpeedRange", Range1D());
    useRotateDirection_ = j.value("useRotateDirection", false);
    rotateDirection_ = j.value("rotateDirection", Vector3());
    lifeTimeRange_ = j.value("lifeTimeRange", Range1D());
    scaleRange_ = j.value("scaleRange", Range3D());

    // モデルの情報
    useDefaultTexture_ = j.value("useDefaultTexture", true);
    emitModelFilePath_ = j.value("emitModelFilePath", "DefaultAssets/Plane/Plane.obj");
}


// 発生位置の中心を取得
Vector3 Emitter3D::GetCenter() const{
    if(parentGroup_){
        if(EmitterGroup3D* group3D = dynamic_cast<EmitterGroup3D*>(parentGroup_)){
            return center_.translate + group3D->GetPosition();
        }
    }
    return center_.translate;
}