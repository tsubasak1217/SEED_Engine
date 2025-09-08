#include "PlayerDeadEffect.h"
#include <SEED/Lib/Functions/myFunc/MyFunc.h>
#include <SEED/Lib/Functions/MyFunc/Easing.h>

// imgui
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

void PlayerDeadEffect::Initialize(const Vector2& playerPos) {
    // パーティクルの初期化
    particles_.resize(kNumParticles_);
    effects_.resize(kNumParticles_);
    for (int i = 0; i < kNumParticles_; i++) {
        particles_[i] = Sprite("DefaultAssets/white1x1.png");

        particles_[i].anchorPoint = Vector2(0.5f);
        particles_[i].layer = 5;
        particles_[i].color = Vector4(1.0f, 0.5f, 0.5f, 1.0f);
        particles_[i].blendMode = BlendMode::ADD;
        effects_[i].isAlive_ = false;
        effects_[i].lifetime = Timer(1.0f);
        effects_[i].position = Vector2(0.0f);
        effects_[i].scale = { 1.0f,1.0f };
        effects_[i].Velocity = Vector2(0.0f);
        effects_[i].color = Vector4(0.0f, 0.5f, 0.5f, 1.0f);
    }
    // エミッタの範囲
    emitaria_.min = Vector2(-30.0f, -30.0f);
    emitaria_.max = Vector2(30.0f, 30.0f);
    emitCenter_ = playerPos;
    emitTimer_ = Timer(0.001f);
    particleSize_ = Vector2(3.0f, 7.0f);
}

void PlayerDeadEffect::Emit() {

    //ランダム色の生成
    //イエロー
    colorPattern_[0] = { 1.0f,1.0f,0.0f };
    //マゼンタ
    colorPattern_[1] = { 1.0f,0.0f,1.0f };
    //シアン
    colorPattern_[2] = { 0.0f,1.0f,1.0f};

    for (int i = 0; i < kNumParticles_; i++) {
        if (!effects_[i].isAlive_) {
            // エフェクト情報の初期化
            effects_[i].isAlive_ = true;
            effects_[i].lifetime = Timer(lifetime_);
            effects_[i].position = emitCenter_ + Vector2(
                MyFunc::Random(emitaria_.min.x, emitaria_.max.x),
                MyFunc::Random(emitaria_.min.y, emitaria_.max.y)
            );
            effects_[i].scale = { MyFunc::Random(1.0f, 20.5f),MyFunc::Random(1.0f, 12.5f) };
 
             effects_[i].color = { colorPattern_[i].x,colorPattern_[i].y,colorPattern_[i].z,1.0f};

            
            break; // 一度に一つだけ発生させる
        }
    }
}

void PlayerDeadEffect::Update(Timer& timer) {

    timer_ = timer;
    //時間経過に合わせて確率を変化させる
    float t = Easing::Ease[Easing::Type::InOutQuint](timer_.GetProgress());

    int parcent = int(MyMath::Lerp(float(emitParcentMax), float(emitParcentMin), t));
    // エミットタイマー更新
    emitTimer_.Update();
    if(emitTimer_.IsFinished()){

        int randomNum = MyFunc::Random(1, 100);


        if(randomNum <= parcent){
            Emit();
        }

        //Emit();
        emitTimer_.Reset();
    }

    for (int i = 0; i < kNumParticles_; i++) {
        if (effects_[i].isAlive_) {
            // タイマー更新
            effects_[i].lifetime.Update();
            if (effects_[i].lifetime.IsFinished()) {
                // 寿命が尽きたら非アクティブにする
                effects_[i].isAlive_ = false;
                continue;
            }
            // 位置の更新
            //effects_[i].position += effects_[i].Velocity * ClockManager::DeltaTime();

        }
    }
}

void PlayerDeadEffect::Draw(bool isDead) {
    if (isDead == false) {
        return; // 死亡中以外は描画しない
    }
    for (int i = 0; i < kNumParticles_; i++) {
        if (effects_[i].isAlive_) {
            particles_[i].transform.translate = effects_[i].position;
            particles_[i].size = particleSize_ * effects_[i].scale;
            particles_[i].color = Vector4(effects_[i].color.x, effects_[i].color.y, effects_[i].color.z, 1.0f);
            particles_[i].Draw();
        }
    }
}

void PlayerDeadEffect::Edit(bool isDead) {
    ImGui::SeparatorText("PlayerDeadEffect");
    ImGui::DragFloat2("emitCenter", &emitCenter_.x, 0.1f);
    ImGui::DragFloat2("emitaria_min", &emitaria_.min.x, 0.1f);
    ImGui::DragFloat2("emitaria_max", &emitaria_.max.x, 0.1f);
    ImGui::DragFloat("lifetime", &lifetime_, 0.1f, 0.1f, 5.0f);
    if (isDead) {
        for (int i = 0; i < kNumParticles_; i++) {
            ImGui::DragFloat2(("effect[" + std::to_string(i) + "]_position").c_str(), &effects_[i].position.x, 0.1f);
            ImGui::DragFloat(("effect[" + std::to_string(i) + "]_scale").c_str(), &effects_[i].scale.x, 0.1f, 0.1f, .0f);
            ImGui::Checkbox(("effect[" + std::to_string(i) + "]_isAlive").c_str(), &effects_[i].isAlive_);
        }
    }

}