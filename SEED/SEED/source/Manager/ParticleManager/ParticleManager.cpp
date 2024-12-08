#include "ParticleManager.h"
#include "ImGuiManager.h"
#include "SEED.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                            statich変数の初期化                                                  */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ParticleManager* ParticleManager::instance_ = nullptr;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                                初期化・終了関数                                                  */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// デストラクタ
/// </summary>
ParticleManager::~ParticleManager(){
    delete instance_;
    instance_ = nullptr;
}


/// <summary>
/// インスタンスを取得
/// </summary>
/// <returns></returns>
ParticleManager* ParticleManager::GetInstance(){
    if(!instance_){
        instance_ = new ParticleManager();
    }
    return instance_;
}

/// <summary>
/// 初期化
/// </summary>
void ParticleManager::Initialize(){
    // インスタンスを取得
    GetInstance();
    // Jsonファイルから読み込み
    instance_->HotReload();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                                     更新処理                                                   */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::Update(){


#ifdef _DEBUG
    ImGui::Begin("ParticleEditor");
    ImGui::Checkbox("isFieldVisible", &instance_->isFieldVisible_);
    instance_->EditEmitter();
    ImGui::End();



#endif // _DEBUG

    ////////////////////////////////////////////
    // エミッターの更新
    ////////////////////////////////////////////
    for(auto& emitter : instance_->emitters_){
        emitter->Update();

        // 発生命令が出ていればパーティクルを発生させる
        if(emitter->emitOrder == true){
            Emit(*emitter);
        }
    }

    ////////////////////////////////////////////
    // パーティクルの更新
    ////////////////////////////////////////////
    for(auto& particle : instance_->particles_){
        particle->Update();
    }

    ////////////////////////////////////////////
    // パーティクルとフィールドの衝突判定
    ////////////////////////////////////////////
    if(instance_->isFieldActive_){
        instance_->CollisionParticle2Field();
    }

    ////////////////////////////////////////////
    // 死んでいる要素の削除
    ////////////////////////////////////////////
    instance_->particles_.remove_if([](auto& particle){
        return !particle->GetIsAlive();
    });

    instance_->emitters_.remove_if([](auto& emitter){
        return !emitter->isAlive;
    });
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                                   描画関数                                                      */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::Draw(){

    // パーティクルの描画
    for(auto& particle : instance_->particles_){
        particle->Draw();
    }

#ifdef _DEBUG
    if(instance_->isFieldVisible_){
        // 加速フィールドの描画
        for(auto& accelerationField : instance_->accelerationFields_){
            AABB area;
            area.center = (accelerationField->range.min + accelerationField->range.max) * 0.5f;
            area.halfSize = (accelerationField->range.max - accelerationField->range.min) * 0.5f;
            SEED::DrawAABB(area, { 1.0f,0.0f,0.0f,1.0f });
        }

        // エミッターの描画
        for(auto& emitter : instance_->emitters_){
            AABB area;
            area.center = emitter->center;
            area.halfSize = emitter->emitRange * 0.5f;
            SEED::DrawAABB(area, { 0.0f,0.0f,1.0f,1.0f });
        }
    }
#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                              領域を追加する関数                                                  */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// 加速フィールドを作成する
/// </summary>
/// <param name="range">フィールドの範囲</param>
/// <param name="force">加速度</param>
void ParticleManager::CreateAccelerationField(const Range3D& range, const Vector3& force){
    instance_->accelerationFields_.emplace_back(
        std::make_unique<AccelerationField>(force, range)
    );
}

/// <summary>
/// エミッターを追加する
/// </summary>
/// <param name="emitter"></param>
void ParticleManager::AddEmitter(const Emitter& emitter){
    instance_->emitters_.emplace_back(std::make_unique<Emitter>(emitter));
}


/// <summary>
/// パーティクルを発生させる(直接)
/// </summary>
/// <param name="emitType"></param>
/// <param name="particleType"></param>
/// <param name="positionRange"></param>
/// <param name="radiusRange"></param>
/// <param name="speedRange"></param>
/// <param name="lifeTimeRange"></param>
/// <param name="colors"></param>
/// <param name="interval"></param>
/// <param name="numEmitEvery"></param>
/// <param name="blendMode"></param>
/// <param name="numEmitMax"></param>
void ParticleManager::AddEmitter(
    EmitType emitType, ParticleType particleType,
    const Vector3& center, const Vector3& emitRange, const Range1D& radiusRange,
    const Range1D& speedRange, const Range1D& lifeTimeRange,
    const std::initializer_list<Vector4>& colors,
    float interval, int32_t numEmitEvery, BlendMode blendMode, int32_t numEmitMax
){
    Emitter emitter;
    emitter.emitType = emitType;
    emitter.particleType = particleType;
    emitter.center = center;
    emitter.emitRange = emitRange;
    emitter.radiusRange = radiusRange;
    emitter.speedRange = speedRange;
    emitter.lifeTimeRange = lifeTimeRange;
    emitter.colors = colors;
    emitter.interval = interval;
    emitter.numEmitEvery = numEmitEvery;
    emitter.blendMode = blendMode;
    emitter.kMaxEmitCount = numEmitMax;

    instance_->emitters_.emplace_back(std::make_unique<Emitter>(emitter));
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                          パーティクルを追加する関数                                               */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// パーティクルを発生させる
/// </summary>
/// <param name="emitter"></param>
void ParticleManager::Emit(Emitter& emitter){

    if(emitter.emitOrder){

        Range3D emitRange;
        emitRange.min = emitter.center - emitter.emitRange * 0.5f;
        emitRange.max = emitter.center + emitter.emitRange * 0.5f;

        Emit(
            emitter.particleType,
            emitRange,
            emitter.radiusRange,
            emitter.speedRange,
            emitter.lifeTimeRange,
            emitter.colors,
            emitter.numEmitEvery,
            emitter.blendMode
        );

        emitter.emitOrder = false;
    }
}

/// <summary>
/// パーティクルを発生させる
/// </summary>
/// <param name="type">パーティクルの種類</param>
/// <param name="positionRange">発生範囲</param>
/// <param name="radiusRange">大きさの範囲</param>
/// <param name="speedRange">スピードの範囲</param>
/// <param name="lifeTime">寿命時間</param>
/// <param name="colors">発生させる色の一覧</param>
/// <param name="count">一度に発生させる数</param>
/// <param name="blendMode">ブレンドモード</param>
void ParticleManager::Emit(
    ParticleType type,
    const Range3D& positionRange,
    const Range1D& radiusRange,
    const Range1D& speedRange,
    const Range1D& lifeTimeRange,
    const std::vector<Vector4>& colors,
    int32_t numEmit,
    BlendMode blendMode
){

    for(int32_t i = 0; i < numEmit; ++i){

        switch(type){
        case ParticleType::kRadial:
            instance_->particles_.emplace_back(
                std::make_unique<RadialParticle>(
                    positionRange, radiusRange, speedRange, lifeTimeRange, colors, blendMode
                ));

            break;
        }
    }
}


/// <summary>
/// ImGuiでのエミッターの編集
/// </summary>
void ParticleManager::EditEmitter(){

    int32_t instanceNum = 0;

    // エミッターごとにクリックすると開けるタブを作成
    for(auto it = instance_->emitters_.begin(); it != instance_->emitters_.end();){

        std::string name = "Emitter_" + std::to_string(instanceNum);
        auto emitter = it->get();

        if(ImGui::CollapsingHeader(name.c_str())){

            // インデントする
            ImGui::Indent();

            // 全般の情報
            if(ImGui::CollapsingHeader("General")){
                ImGui::Indent();

                ImGui::Checkbox("isActive", &emitter->isActive);
                ImGui::DragFloat3("center", &emitter->center.x, 0.05f);
                ImGui::Combo("emitType", (int*)&emitter->emitType, "kOnce\0kInfinite\0kCustom\0");
                ImGui::Combo("particleType", (int*)&emitter->particleType, "kRadial\0");
                ImGui::Combo(
                    "blendMode", (int*)&emitter->blendMode,
                    "NONE\0MULTIPLY\0SUBTRACT\0NORMAL\0ADD\0SCREEN\0"
                );

                ImGui::Unindent();
            }

            ImGui::Separator();

            // 範囲などの情報
            if(ImGui::CollapsingHeader("Range")){
                ImGui::Indent();

                ImGui::Text("-------- Emit --------");
                ImGui::DragFloat3("emitRange", &emitter->emitRange.x, 0.05f);
                ImGui::Text("------- Radius -------");
                ImGui::DragFloat("radiusRange.min", &emitter->radiusRange.min, 0.01f, 0.0f, emitter->radiusRange.max);
                ImGui::DragFloat("radiusRange.max", &emitter->radiusRange.max, 0.01f, emitter->radiusRange.min);
                ImGui::Text("------- Speed -------");
                ImGui::DragFloat("speedRange.min", &emitter->speedRange.min, 0.02f, 0.0f, emitter->speedRange.max);
                ImGui::DragFloat("speedRange.max", &emitter->speedRange.max, 0.02f, emitter->speedRange.min);
                ImGui::Text("------ LifeTime ------");
                ImGui::DragFloat("lifeTimeRange.min", &emitter->lifeTimeRange.min, 0.05f, 0.0f, emitter->lifeTimeRange.max);
                ImGui::DragFloat("lifeTimeRange.max", &emitter->lifeTimeRange.max, 0.05f, emitter->lifeTimeRange.min);

                // 色の設定
                ImGui::Text("-------- Colors --------");
                if(ImGui::CollapsingHeader("ColorList")){
                    ImGui::Indent();

                    // 色のリスト
                    for(int32_t i = 0; i < (int)emitter->colors.size(); ++i){
                        std::string colorName = "color_" + std::to_string(i);
                        ImGui::ColorEdit4(colorName.c_str(), &emitter->colors[i].x);
                        // 削除ボタン
                        if(ImGui::Button("DeleteColor")){
                            if(emitter->colors.size() > 1){
                                emitter->colors.erase(emitter->colors.begin() + i);
                            }
                        }
                    }

                    // 追加ボタン
                    ImGui::Text("-- Add --");
                    if(ImGui::Button("AddColor")){
                        emitter->colors.emplace_back(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
                    }

                    ImGui::Unindent();
                }

                ImGui::Unindent();
            }

            ImGui::Separator();

            // 頻度などの情報
            if(ImGui::CollapsingHeader("Frequency")){
                ImGui::Indent();

                ImGui::DragFloat("emitInterval", &emitter->interval, 0.01f, 0.0f);
                ImGui::DragInt("numEmitEvery", &emitter->numEmitEvery, 1,0,100);
                if(emitter->emitType == EmitType::kCustom){
                    ImGui::DragInt("kMaxEmitCount", &emitter->kMaxEmitCount, 1);
                }

                ImGui::Unindent();
            }

            // 削除ボタン
            if(ImGui::Button("DeleteEmitter")){
                it = instance_->emitters_.erase(it); // 要素を削除し、次の要素を取得
                continue; // 削除したので次の要素に進む
            }

            // インデントを戻す
            ImGui::Unindent();
        }

        ++it;// 次の要素に進む
        ++instanceNum;
    }

    // エミッターの追加ボタン
    ImGui::Text("-- AddEmitter --");
    if(ImGui::Button("AddEmitter")){
        Emitter emitter;
        instance_->emitters_.emplace_back(std::make_unique<Emitter>(emitter));
    }

    // ホットリロード
    ImGui::Text("-- HotReload --");
    if(ImGui::Button("HotReload")){
        HotReload();
    }

    // ファイルに保存
    ImGui::Text("-- SaveToJson --");
    if(ImGui::Button("SaveToJson")){
        SaveToJson();
        // ログのウインドウを出す
        MessageBoxA(nullptr, "Json is Saved", "SaveToJson", MB_OK);
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                                  ファイル入出力                                                 */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// jsonファイルに保存
/// </summary>
void ParticleManager::SaveToJson(){
    nlohmann::json j;

    // エミッターの情報を保存
    for(auto& emitter : instance_->emitters_){
        nlohmann::json emitterJson;

        // 全般の情報
        emitterJson["isActive"] = emitter->isActive;
        emitterJson["emitType"] = (int)emitter->emitType;
        emitterJson["particleType"] = (int)emitter->particleType;
        emitterJson["blendMode"] = (int)emitter->blendMode;
        emitterJson["center"] = { emitter->center.x, emitter->center.y, emitter->center.z };

        // 範囲などの情報
        emitterJson["emitRange"] = { emitter->emitRange.x, emitter->emitRange.y, emitter->emitRange.z };
        emitterJson["radiusRange"] = { emitter->radiusRange.min, emitter->radiusRange.max };
        emitterJson["speedRange"] = { emitter->speedRange.min, emitter->speedRange.max };
        emitterJson["lifeTimeRange"] = { emitter->lifeTimeRange.min, emitter->lifeTimeRange.max };

        // 発生頻度などの情報
        emitterJson["interval"] = emitter->interval;
        emitterJson["numEmitEvery"] = emitter->numEmitEvery;
        emitterJson["kMaxEmitCount"] = emitter->kMaxEmitCount;

        // 色の情報
        for(auto& color : emitter->colors){
            emitterJson["colors"].push_back({ color.x, color.y, color.z, color.w });
        }

        j["emitters"].push_back(emitterJson);
    }

    // ファイルに保存
    std::ofstream ofs("resources/jsons/particle/Emitters.json");
    ofs << j.dump(4);
}

/// <summary>
/// Jsonファイルから読み込み
/// </summary>
void ParticleManager::LoadFromJson(){
    std::ifstream ifs("resources/jsons/particle/Emitters.json");
    if(ifs.fail()){
        MessageBoxA(nullptr, "Json file is not found", "LoadFromJson", MB_OK);
        return;
    }

    nlohmann::json j;
    ifs >> j;

    // エミッターの情報を読み込み
    for(auto& emitterJson : j["emitters"]){
        Emitter emitter;

        // 全般の情報
        emitter.isActive = emitterJson["isActive"];
        emitter.emitType = (EmitType)emitterJson["emitType"];
        emitter.particleType = (ParticleType)emitterJson["particleType"];
        emitter.blendMode = (BlendMode)emitterJson["blendMode"];
        emitter.center = Vector3(
            emitterJson["center"][0], emitterJson["center"][1], emitterJson["center"][2]
        );

        // 範囲などの情報
        emitter.emitRange = Vector3(
            emitterJson["emitRange"][0], emitterJson["emitRange"][1], emitterJson["emitRange"][2]
        );
        emitter.radiusRange = Range1D(
            emitterJson["radiusRange"][0], emitterJson["radiusRange"][1]
        );
        emitter.speedRange = Range1D(
            emitterJson["speedRange"][0], emitterJson["speedRange"][1]
        );
        emitter.lifeTimeRange = Range1D(
            emitterJson["lifeTimeRange"][0], emitterJson["lifeTimeRange"][1]
        );

        // 発生頻度などの情報
        emitter.interval = emitterJson["interval"];
        emitter.numEmitEvery = emitterJson["numEmitEvery"];
        emitter.kMaxEmitCount = emitterJson["kMaxEmitCount"];

        // 色の情報
        emitter.colors.clear();
        for(auto& color : emitterJson["colors"]){
            emitter.colors.emplace_back(Vector4(color[0], color[1], color[2], color[3]));
        }

        instance_->emitters_.emplace_back(std::make_unique<Emitter>(emitter));
    }
}


/// <summary>
/// 起動中にファイルを再読み込み
/// </summary>
void ParticleManager::HotReload(){
    instance_->emitters_.clear();
    LoadFromJson();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                パーティクルと加速フィールドの当たり判定関数                                          */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// パーティクルと加速フィールドの衝突判定
/// </summary>
void ParticleManager::CollisionParticle2Field(){
    for(auto& particle : instance_->particles_){
        for(auto& field : instance_->accelerationFields_){
            if(field->CheckCollision(particle->GetPos())){
                particle->SetAcceleration(field->acceleration);
            }
        }
    }
}


