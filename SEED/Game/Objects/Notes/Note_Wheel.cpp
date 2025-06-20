#include "Note_Wheel.h"

Note_Wheel::Note_Wheel() : Note_Base(){
    noteType_ = NoteType::Wheel;// ノーツの種類をタップに設定
    noteQuad_.get()->lightingType = LIGHTINGTYPE_NONE;// ライティングを無効に

    // GHを取得
    wheelUpGH_ = TextureManager::LoadTexture("Notes/wheel_Up.png");
    wheelDownGH_ = TextureManager::LoadTexture("Notes/wheel_Down.png");
    // UpDownColorを設定(Up:マゼンタ,Down:シアン)
    upDownColor_[0] = {1.0f, 0.0f, 1.0f, 1.0f}; // Magenta
    upDownColor_[1] = { 0.0f, 1.0f, 1.0f, 1.0f }; // Cyan
}

Note_Wheel::~Note_Wheel(){
}

void Note_Wheel::Update(){
}

void Note_Wheel::Draw(float currentTime, float appearLength){
    static Quad noteFloorRect;
    static Quad noteDirectionRect;
    float timeRatio = 1.0f - ((time_ - currentTime) / appearLength);

    // 描画用の矩形を計算
    noteFloorRect = PlayField::GetInstance()->GetWheelFloorQuad(timeRatio, layer_,0.005f);
    noteDirectionRect = PlayField::GetInstance()->GetWheelDirectionQuad(timeRatio, layer_);

    // 頂点設定
    for(int i = 0; i < 4; i++){
        noteFloorRect.localVertex[i].z += zOffset_;
        noteDirectionRect.localVertex[i].z += zOffset_;
    }

    // 色やテクスチャの設定
    if(direction_ == UpDown::UP){
        // 上向きのノーツ
        noteDirectionRect.GH = wheelUpGH_;
        noteFloorRect.color = upDownColor_[0]; // マゼンタ
    } else{
        // 下向きのノーツ
        noteDirectionRect.GH = wheelDownGH_;
        noteFloorRect.color = upDownColor_[1]; // シアン
    }

    noteDirectionRect.blendMode = BlendMode::ADD; // ブレンドモードを加算に設定
    noteFloorRect.blendMode = BlendMode::ADD; // ブレンドモードを加算に設定
    SEED::DrawQuad(noteFloorRect);
    SEED::DrawQuad(noteDirectionRect);
}

////////////////////////////////////////////////////////////////////
// タップノーツの判定
////////////////////////////////////////////////////////////////////
Judgement::Evaluation Note_Wheel::Judge(float dif){
    // 入力情報を取得
    static auto input = PlayerInput::GetInstance();
    bool isTap = input->GetIsTap();

    // タップしていないなら、判定しない
    if(!isTap){
        return Judgement::Evaluation::NONE;
    }

    // 押したレーンを取得
    auto& tapLane = input->GetTapLane();

    // 自身のレーンと押したレーンに含まれているか
    uint32_t lane = lane_ % PlayField::kKeyCount_;

    if(tapLane.find(lane) != tapLane.end()){
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
