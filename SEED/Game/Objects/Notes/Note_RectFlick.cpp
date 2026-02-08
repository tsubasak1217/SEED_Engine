#include "Note_RectFlick.h"
#include <SEED/Lib/Functions/Easing.h>

Note_RectFlick::Note_RectFlick() : Note_Base(){
    noteType_ = NoteType::RectFlick;
}

Note_RectFlick::~Note_RectFlick(){
}

void Note_RectFlick::Update(){
}

void Note_RectFlick::Draw(float currentTime, float appearLength){
    SEED::Topology::Quad2D q;
    float timeRatio = 1.0f - ((time_ - currentTime) / (appearLength * 0.2f));
    float alpha = 0.0f;

    std::vector<SEED::GeneralEnum::DIRECTION8> dirs;
    if(laneBit_ & LaneBit::RECTFLICK_LT){ dirs.push_back(SEED::GeneralEnum::DIRECTION8::LEFTTOP); }
    if(laneBit_ & LaneBit::RECTFLICK_LB){ dirs.push_back(SEED::GeneralEnum::DIRECTION8::LEFTBOTTOM); }
    if(laneBit_ & LaneBit::RECTFLICK_RT){ dirs.push_back(SEED::GeneralEnum::DIRECTION8::RIGHTTOP); }
    if(laneBit_ & LaneBit::RECTFLICK_RB){ dirs.push_back(SEED::GeneralEnum::DIRECTION8::RIGHTBOTTOM); }

    if(timeRatio <= 1.0f){
        alpha = SEED::Methods::Easing::OutSine(std::clamp(timeRatio, 0.0f, 1.0f));
    } else{
        alpha = 1.0f - SEED::Methods::Easing::OutSine(std::clamp((timeRatio - 1.0f) / 0.2f, 0.0f, 1.0f));
    }

    // フリック矩形の描画
    for(int i = 0; i < dirs.size(); ++i){
        // 描画用の四角形を取得
        q = PlayField::GetInstance()->GetRectFlickQuad(timeRatio, dirs[i], 0.05f);
        // 描画
        q.color = { 1.0f,1.0f,0.0f,alpha };
       SEED::Instance::DrawQuad2D(q);

        // 予告矩形の描画
        float ease = SEED::Methods::Easing::OutQuart(std::clamp(timeRatio, 0.0f, 1.0f));
        q = PlayField::GetInstance()->GetRectFlickQuad(1.0f + 0.05f * ease, dirs[i], 0.1f * ease);
        q.color = { 1.0f,0.0f,0.0f,alpha };
        q.blendMode = SEED::BlendMode::ADD;
       SEED::Instance::DrawQuad2D(q);
    }
}

Judgement::Evaluation Note_RectFlick::Judge(float curTime){

    // 時間差を計算
    float signedDif = curTime - time_;
    float dif = std::fabsf(signedDif);

    // 入力情報を取得
    static auto input = PlayerInput::GetInstance();
    SEED::GeneralEnum::DIRECTION8 flickDirection = input->GetRectFlickDirection();

    // フリックの方向が自身のレーンと一致しているか
    if(!CheckBit(flickDirection)){
        return Judgement::Evaluation::MISS;
    }

    // ノーツの判定(判定はゆるめにしてパーフェクトかミスだけにする)
    if(dif > Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GOOD)){
        // MISS
        return Judgement::Evaluation::MISS;
    } else{
        // PERFECT
        return Judgement::Evaluation::PERFECT;
    }
}

// JSON変換関数
nlohmann::json Note_RectFlick::ToJson(){
    nlohmann::json json = Note_Base::ToJson(); // 基本情報を取得
    json["noteType"] = "RectFlick"; // タイプを指定
    return json; // JSONを返す
}

// jsonから読み込む関数
void Note_RectFlick::FromJson(const nlohmann::json& json){
    Note_Base::FromJson(json); // 基本情報を読み込む
}


// 範囲内のフリックか、ビットで確認する関数
bool Note_RectFlick::CheckBit(SEED::GeneralEnum::DIRECTION8 dir) const{
    LaneBit bit;
    switch(dir){
    case SEED::GeneralEnum::DIRECTION8::LEFTTOP:
        bit = LaneBit::RECTFLICK_LT;
        break;
    case SEED::GeneralEnum::DIRECTION8::LEFTBOTTOM:
        bit = LaneBit::RECTFLICK_LB;
        break;
    case SEED::GeneralEnum::DIRECTION8::RIGHTTOP:
        bit = LaneBit::RECTFLICK_RT;
        break;
    case SEED::GeneralEnum::DIRECTION8::RIGHTBOTTOM:
        bit = LaneBit::RECTFLICK_RB;
        break;
    default:
        return false;
    }

    return (laneBit_ & bit);// ビットが立っているか確認
}

#ifdef _DEBUG
void Note_RectFlick::Edit(){
    // 基本情報の編集
    Note_Base::Edit();
    ImGui::Separator();
    // ホールドノーツの情報の編集
    SEED::ImFunc::ComboPair("フリック方向", laneBit_,
        {
            {"", LaneBit::RECTFLICK_LT},
            {"左上(ワイド)", LaneBit::RECTFLICK_LT_EX},
            {"左下", LaneBit::RECTFLICK_LB},
            {"(ワイド)", LaneBit::RECTFLICK_LB_EX},
            {"↗", LaneBit::RECTFLICK_RT},
            {"↗(ワイド)", LaneBit::RECTFLICK_RT_EX},
            {"↘", LaneBit::RECTFLICK_RB},
            {"↘(ワイド)", LaneBit::RECTFLICK_RB_EX},
            {"↑", LaneBit::RECTFLICK_UP},
            {"↓", LaneBit::RECTFLICK_DOWN},
            {"←", LaneBit::RECTFLICK_LEFT},
            {"→", LaneBit::RECTFLICK_RIGHT},
            {"全体", LaneBit::RECTFLICK_ALL},
        }
    );
}
#endif // _DEBUG