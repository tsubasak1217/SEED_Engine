#include "Emitter2D.h"
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterGroup2D.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>
#include <SEED/Source/Manager/ModelManager/ModelManager.h>
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterBase.h>

namespace SEED{
    Emitter2D::Emitter2D() : EmitterBase(){
        // カーブを初期化
        scaleCurve_ = Curve(CurveChannel::VECTOR2);
        velocityCurve_ = Curve(CurveChannel::FLOAT);
        rotateCurve_ = Curve(CurveChannel::FLOAT);
        colorCurve_ = Curve(CurveChannel::VECTOR4);
        positionInterpolationCurve_ = Curve(CurveChannel::FLOAT);
    }


#ifdef _DEBUG
    void Emitter2D::Edit(){

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



        // Guizmoの登録
        if(useGuizmo_){

            // スケールが1でない場合に範囲をスケーリングする
            if(Input::IsPressMouse(MOUSE_BUTTON::LEFT)){
                static Vector2 prevRange;
                if(Input::IsTriggerMouse(MOUSE_BUTTON::LEFT)){
                    prevRange = emitRange_;
                }
                if(center_.scale != Vector2(1.0f)){
                    emitRange_ = prevRange * center_.scale;
                }
            } else{
                center_.scale = Vector2(1.0f);
            }

            // 親グループの位置を考慮する
            Matrix3x3 parentMat = Methods::Matrix::IdentityMat3();
            if(parentGroup_){
                if(auto group2D = dynamic_cast<EmitterGroup2D*>(parentGroup_)){
                    parentMat = Methods::Matrix::TranslateMatrix(group2D->GetPosition());
                }
            }

            // 登録
            ImGuiManager::RegisterGuizmoItem(&center_, parentMat.ToMat4x4());
        }

        // 出現範囲の可視化
        Topology::OBB2D rangeBox;
        rangeBox.center = GetCenter();
        rangeBox.halfSize = emitRange_ * 0.5f;
        rangeBox.rotate = center_.rotate;
        SEED::Instance::DrawOBB2D(rangeBox, { 0.0f,1.0f,1.0f,1.0f });

        ImGui::Unindent();
    }


    /*------------------------*/
    /*        全般の情報       */
    /*------------------------*/
    void Emitter2D::EditGeneral(){

        // エミッター自身のトランスフォーム
        ImGui::SeparatorText("発生範囲の設定");
        ImGui::DragFloat2("エミッター中心座標" + idTag_, &center_.translate.x);
        ImGui::DragFloat2("発生範囲" + idTag_, &emitRange_.x);
        ImGui::DragFloat("エミッター回転角度" + idTag_, &center_.rotate, 0.005f);
        ImGui::Spacing();

        // 発生タイプ
        ImGui::SeparatorText("発生タイプの設定");
        ImFunc::Combo("発生タイプ" + idTag_, emitType_, { "一度のみ","永続","指定回数" });
        ImFunc::HelpTip("エミッターが終了するまでの発生回数");
        if(emitType_ == EmitType::kCustom){
            ImGui::DragInt("発生回数" + idTag_, &kMaxEmitCount_, 1);
        }
    }


    /*------------------------*/
    /*      範囲などの情報      */
    /*------------------------*/
    void Emitter2D::EditTransformSettings(){

        // 大きさ関連の情報-----------------------------------------
        if(ImFunc::CollapsingHeader("大きさ・スケール" + idTag_)){
            ImGui::Indent();
            ImGui::SeparatorText("半径");
            ImGui::DragFloat("最小半径" + idTag_, &radiusRange_.min, 0.01f, 0.0f, radiusRange_.max);
            ImGui::DragFloat("最大半径" + idTag_, &radiusRange_.max, 0.01f, radiusRange_.min, FLT_MAX);

            ImGui::SeparatorText("スケール");
            ImGui::DragFloat2("最小倍率(x,y,x)" + idTag_, &scaleRange_.min.x, 0.005f, 0.0f, scaleRange_.max.x);
            ImGui::DragFloat2("最大倍率(x,y,x)" + idTag_, &scaleRange_.max.x, 0.005f, scaleRange_.min.x, FLT_MAX);

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
            if(ImGui::DragFloat2("基礎となる発射方向" + idTag_, &baseDirection_.x, 0.01f)){
                baseDirection_ = Methods::Math::Normalize(baseDirection_);
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
                ImGui::DragFloat2("ゴール位置" + idTag_, &goalPosition_.x, 0.05f);

                // 移動速度のカーブ編集
                if(ImGui::CollapsingHeader("目標点までの補間カーブ" + idTag_)){
                    ImGui::Indent();
                    positionInterpolationCurve_.Edit();
                    ImGui::Unindent();
                }

                // ゴール位置の可視化
                Vector2 pos = GetCenter() + goalPosition_;
                SEED::Instance::DrawHexagon(pos, 1.0f, 0.0f, { 1.0f,0.0f,0.0f,1.0f });
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
            ImGui::Checkbox("回転の初期化値をランダムにするか" + idTag_, &isRoteteRandomInit_);
            ImGui::Checkbox("回転するか" + idTag_, &isUseRotate_);
            if(isUseRotate_){
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
    void Emitter2D::EditTextureAndDrawSettings(){

        // BlendMode, CullingMode,LightingTypeの設定
        ImGui::Text("-------- 描画設定 --------");
        ImFunc::Combo("ブレンドモード" + idTag_, blendMode_, { "NONE","MULTIPLY","SUBTRACT","NORMAL","ADD","SCREEN" });
        ImFunc::Combo("描画場所" + idTag_, (int&)drawLocation_, { "背景" ,"前景" }, 1);
        ImGui::DragInt("描画レイヤー" + idTag_, &layer_, 0.1f, -100, 100);
        ImGui::Checkbox("ビュー行列を適用するか" + idTag_, &isApplyViewMatrix_);
        ImFunc::HelpTip("ビュー行列を適用すると、パーティクルがカメラ面に固定されなくなります");


        // テクスチャの設定
        ImGui::Text("-------- テクスチャ --------");
        if(ImGui::CollapsingHeader("テクスチャ設定" + idTag_)){
            ImGui::Indent();

            // エミッターに追加されたテクスチャのリスト
            static ImVec2 buttonSize = ImVec2(50, 50);
            float curRightX = ImGui::GetCursorPosX();
            float availRegionX = ImGui::GetContentRegionAvail().x;

            if(ImGui::CollapsingHeader("カスタムテクスチャ" + idTag_)){
                ImGui::Indent();

                // 画像の一覧から選択したものをエミッターのテクスチャリストに追加
                for(int32_t i = 0; i < texturePaths_.size(); i++){

                    // テクスチャの表示(押したら消す)
                    if(ImGui::ImageButton(Methods::String::PtrToStr(&texturePaths_[i]), textureDict_[texturePaths_[i]], buttonSize)){
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
            std::string selected = ImFunc::FolderView("追加テクスチャを選択", curPath, { ".png", ".jpg" }, rootPath);

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

        // 基底クラスの情報を書き込む
        j.update(EmitterBase::ExportToJson());

        // 全般の情報
        j["emitterType"] = "Emitter2D";
        j["center"] = center_.translate;
        j["rotate"] = center_.rotate;

        // 範囲・パラメーターなどの情報
        j["emitRange"] = emitRange_;
        j["radiusRange"] = radiusRange_;
        j["baseDirection"] = baseDirection_;
        j["directionRange"] = directionRange_;
        j["goalPosition"] = goalPosition_;
        j["speedRange"] = speedRange_;
        j["rotateSpeedRange"] = rotateSpeedRange_;
        j["lifeTimeRange"] = lifeTimeRange_;
        j["gravity"] = gravity_;
        j["scaleRange"] = scaleRange_;

        // 描画設定
        j["layer"] = layer_;
        j["drawLocation"] = (int)drawLocation_;
        j["isApplyViewMatrix"] = isApplyViewMatrix_;

        return j;
    }

    ///////////////////////////////////////////////////////////
    // 読み込み
    ///////////////////////////////////////////////////////////
    void Emitter2D::LoadFromJson(const nlohmann::json& j){

        // 基底クラスの情報を読み込む
        EmitterBase::LoadFromJson(j);

        // 全般の情報
        center_.translate = j.value("center", Vector2(0.0f));
        center_.rotate = j.value("rotate", 0.0f);

        // 範囲やパラメーターなどの情報
        emitRange_ = j.value("emitRange", Vector2());
        radiusRange_ = j.value("radiusRange", Range1D());
        goalPosition_ = j.value("goalPosition", Vector2());
        baseDirection_ = j.value("baseDirection", Vector2(0.0f, -1.0f));
        directionRange_ = j.value("directionRange", 1.0f);
        speedRange_ = j.value("speedRange", Range1D());
        rotateSpeedRange_ = j.value("rotateSpeedRange", Range1D());
        lifeTimeRange_ = j.value("lifeTimeRange", Range1D());
        scaleRange_ = j.value("scaleRange", Range2D());

        // 描画設定
        layer_ = j.value("layer", 0);
        drawLocation_ = (DrawLocation)j.value("drawLocation", 2);
        isApplyViewMatrix_ = j.value("isApplyViewMatrix", false);
    }


    // 発生位置の中心を取得
    Vector2 Emitter2D::GetCenter() const{
        if(parentGroup_){
            if(EmitterGroup2D* group2D = dynamic_cast<EmitterGroup2D*>(parentGroup_)){
                return center_.translate + group2D->GetPosition();
            }
        }
        return center_.translate;
    }
}