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

    float timeRatio = 1.0f - ((time_ - currentTime) / appearLength);
    uv_translate_.y += 2.0f * ClockManager::DeltaTime();

    // 描画設定
    noteDirectionRect.blendMode = BlendMode::ADD; // ブレンドモードを加算に設定
    noteFloorRect.blendMode = BlendMode::ADD; // ブレンドモードを加算に設定
    noteAuraRect.blendMode = BlendMode::ADD; // ブレンドモードを加算に設定

    // 描画用の矩形を計算
    noteFloorRect = PlayField::GetInstance()->GetWheelFloorQuad(timeRatio, layer_, 0.005f * (4.0f/appearLength));
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
        noteAuraRect.color.value.w = 0.5f;
    } else{
        noteFloorRect.color = upDownColor_[1]; // シアン
        noteAuraRect.color = upDownColor_[1];
        noteAuraRect.color.value.w = 0.5f;
    }

    // uvをスクロールする
    float directionScale = 2.0f;
    if(direction_ == UpDown::DOWN){
        directionScale *= -1.0f; // 下方向ならUVを反転
    }
    if(layer_ == UpDown::DOWN){
        directionScale *= -1.0f; // レイヤーが下ならUVを反転
    }

    noteDirectionRect.uvTransform = AffineMatrix({ 1.0f,directionScale,1.0f }, { 0.0f,0.0f,0.0f }, uv_translate_);

    // テクスチャの設定
    noteAuraRect.GH = wheelAuraGH_;
    noteDirectionRect.GH = wheelDirectionGH_;

    // 描画
   SEED::Instance::DrawQuad(noteFloorRect);
   SEED::Instance::DrawQuad(noteDirectionRect);
   SEED::Instance::DrawQuad(noteAuraRect);
}

////////////////////////////////////////////////////////////////////
// タップノーツの判定
////////////////////////////////////////////////////////////////////
Judgement::Evaluation Note_Wheel::Judge(float curTime){

    // 時間差を計算
    float signedDif = curTime - time_;
    float dif = std::fabsf(signedDif);

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

    // MISS以外はPERFECT扱いにする
    if(int32_t(lane) & laneBit_){
        // ノーツの判定
        if(dif > Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GOOD)){
            // MISS
            return Judgement::Evaluation::MISS;

        } else if(dif > Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GREAT)){
            // GOOD
            return Judgement::Evaluation::GREAT;

        } else if(dif > Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::PERFECT)){
            // GREAT
            return Judgement::Evaluation::PERFECT;

        } else{
            // PERFECT
            return Judgement::Evaluation::PERFECT;
        }
    }

    // 押したレーンに含まれていないなら、MISS
    return Judgement::Evaluation::MISS;
}

nlohmann::json Note_Wheel::ToJson(){
    nlohmann::json json = Note_Base::ToJson();
    json["noteType"] = "wheel"; // ノーツの種類を保存
    json["direction"] = (int)direction_; // フリック方向を保存
    return json;
}

void Note_Wheel::FromJson(const nlohmann::json& json){
    Note_Base::FromJson(json);
    direction_ = (UpDown)json["direction"];
}

#ifdef _DEBUG
void Note_Wheel::Edit(){
    // 基本情報の編集
    Note_Base::Edit();
    ImGui::Separator();
    // ホールドノーツの情報の編集
    if(ImFunc::ComboPair("フリック方向", direction_,
        {
            {"↑", UpDown::UP},
            {"↓", UpDown::DOWN},
        }
        )){
        // フリック方向が変更された場合、レーンビットを更新
        laneBit_ = direction_ == UpDown::UP ? LaneBit::WHEEL_UP : LaneBit::WHEEL_DOWN;
    }
}
#endif // _DEBUG
