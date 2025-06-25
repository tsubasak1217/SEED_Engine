#include "Note_Wheel.h"

Note_Wheel::Note_Wheel() : Note_Base(){
    noteType_ = NoteType::Wheel;// ノーツの種類をタップに設定
    noteQuad_.get()->lightingType = LIGHTINGTYPE_NONE;// ライティングを無効に

    // GHを取得
    wheelDirectionGH_ = TextureManager::LoadTexture("Notes/wheel_Direction.png");
    wheelAuraGH_ = TextureManager::LoadTexture("Notes/wheel_aura.png");
    // UpDownColorを設定(Up:マゼンタ,Down:シアン)
    upDownColor_[0] = { 1.0f, 0.0f, 1.0f, 1.0f }; // Magenta
    upDownColor_[1] = { 0.0f, 1.0f, 1.0f, 1.0f }; // Cyan
}

Note_Wheel::~Note_Wheel(){
}

void Note_Wheel::Update(){
}

void Note_Wheel::Draw(float currentTime, float appearLength){
    static Quad noteFloorRect;
    static Quad noteDirectionRect;
    static Quad noteAuraRect;
    static Vector3 uv_translate;
    float timeRatio = 1.0f - ((time_ - currentTime) / appearLength);
    uv_translate.y += 0.3f * ClockManager::DeltaTime();

    // 描画設定
    noteDirectionRect.blendMode = BlendMode::ADD; // ブレンドモードを加算に設定
    noteFloorRect.blendMode = BlendMode::ADD; // ブレンドモードを加算に設定
    noteAuraRect.blendMode = BlendMode::ADD; // ブレンドモードを加算に設定

    // 描画用の矩形を計算
    noteFloorRect = PlayField::GetInstance()->GetWheelFloorQuad(timeRatio, layer_, 0.005f);
    noteDirectionRect = PlayField::GetInstance()->GetWheelDirectionQuad(timeRatio, layer_);
    noteAuraRect = noteDirectionRect;

    // 頂点設定
    for(int i = 0; i < 4; i++){
        noteFloorRect.localVertex[i].z += zOffset_;
    }

    // 色やテクスチャの設定
    if(direction_ == UpDown::UP){
        noteFloorRect.color = upDownColor_[0]; // マゼンタ
        noteAuraRect.color = upDownColor_[0];
        noteAuraRect.color.w = 0.5f;
    } else{
        noteFloorRect.color = upDownColor_[1]; // シアン
        noteAuraRect.color = upDownColor_[1];
        noteAuraRect.color.w = 0.5f;
    }

    // uvをスクロールする
    noteDirectionRect.uvTransform = AffineMatrix({ 1.0f,2.0f,1.0f }, {0.0f,0.0f,0.0f}, uv_translate);

    // テクスチャの設定
    noteAuraRect.GH = wheelAuraGH_;
    noteDirectionRect.GH = wheelDirectionGH_;

    // 描画
    SEED::DrawQuad(noteFloorRect);
    SEED::DrawQuad(noteDirectionRect);
    SEED::DrawQuad(noteAuraRect);
}

////////////////////////////////////////////////////////////////////
// タップノーツの判定
////////////////////////////////////////////////////////////////////
Judgement::Evaluation Note_Wheel::Judge(float dif){
    // 入力情報を取得
    static auto input = PlayerInput::GetInstance();
    bool isScroll = input->GetIsWheelTrigger();

    // タップしていないなら、判定しない
    if(!isScroll){
        return Judgement::Evaluation::NONE;
    }

    // 自身のレーンと押したレーンに含まれているか
    LaneBit lane = input->GetWheelScrollDirection() == UpDown::UP ?
        LaneBit::WHEEL_UP : LaneBit::WHEEL_DOWN;

    if(int32_t(lane) & laneBit_){
        // ノーツの判定
        if(dif > Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GOOD)){
            // MISS
            return Judgement::Evaluation::MISS;

        } else if(dif > Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GREAT)){
            // GOOD
            return Judgement::Evaluation::GOOD;

        } else if(dif > Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::PERFECT)){
            // GREAT
            return Judgement::Evaluation::GREAT;

        } else{
            // PERFECT
            return Judgement::Evaluation::PERFECT;
        }
    }

    // 押したレーンに含まれていないなら、MISS
    return Judgement::Evaluation::MISS;
}
