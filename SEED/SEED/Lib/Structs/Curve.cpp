#include "Curve.h"
#include <SEED/Lib/Functions/MyFunc/MyMath.h>

Curve::Curve(){
    for(auto& curve : curves_){
        curve.clear();
        curve.push_back(Vector2(0.0f, 0.0f));
        curve.push_back(Vector2(1.0f, 1.0f));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// t0~1の範囲で補間した値を取得
////////////////////////////////////////////////////////////////////////////////////////////////////////
float Curve::GetValue(float t, int channel) const{

    // 引数をクランプ
    t = std::clamp(t, 0.0f, 1.0f);
    channel = std::clamp(channel, 0, int(curves_.size()) - 1);

    // チャンネルのポイントを取得
    const auto& points = curves_[channel];
    if(points.size() == 0){ return 0.0f; }
    if(points.size() == 1){ return points[0].y; }

    // tが存在している区間を探す
    for(size_t i = 0; i < points.size() - 1; ++i){
        if(t >= points[i].x && t <= points[i + 1].x){
            // 区間内に存在している場合
            float localT = (t - points[i].x) / (points[i + 1].x - points[i].x);
            float ease = Easing::Ease[curveType_](localT);
            return MyMath::Lerp(points[i].y, points[i + 1].y, ease);
        }
    }

    // 見つからなかった場合は0を返す
    return 0.0f;
}

Vector2 Curve::GetValue2(float t) const{
    return Vector2(GetValue(t, 0), GetValue(t, 1));
}

Vector3 Curve::GetValue3(float t) const{
    return Vector3(GetValue(t, 0), GetValue(t, 1), GetValue(t, 2));
}

Vector4 Curve::GetValue4(float t) const{
    return Vector4(GetValue(t, 0), GetValue(t, 1), GetValue(t, 2), GetValue(t, 3));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
// 編集関数
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Curve::Edit(){
    // ウィンドウを出さずに編集
    editor_.EditGUI(false);
    // 編集データを適用
    editor_.EdittingDataToCurve(*this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Jsonから読み込み
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Curve::FromJson(const nlohmann::json& json){
    // jsonが空の場合は何もしない
    if(json.is_null() || json.empty()){ return; }

    // チャンネル数
    channel_ = static_cast<CurveChannel>(json.value("channel",0));

    // カーブタイプ
    curveType_ = static_cast<Easing::Type>(json.value("type", 0));

    // 単位ボーダー
    unitValueBorder_ = json.value("unitBorder", 1.0f);// エディタに渡すために内部に持っておく

    // カーブのポイントを読み込み
    if(json.contains("curves")){
        for(int i = 0; i < curves_.size(); ++i){
            if(i >= json["curves"].size()){ break; }
            curves_[i].clear();
            for(const auto& pointJson : json["curves"][i]){
                if(pointJson.contains("x") && pointJson.contains("y")){
                    Vector2 p;
                    p.x = pointJson["x"].get<float>();
                    p.y = pointJson["y"].get<float>();
                    curves_[i].push_back(p);
                }
            }
        }
    }

    // エディタの初期化
    editor_.Initialize(*this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Jsonに変換
////////////////////////////////////////////////////////////////////////////////////////////////////////
nlohmann::json Curve::ToJson()const{
    nlohmann::json j;
    // チャンネル数
    j["channel"] = static_cast<int>(channel_);
    // カーブタイプ
    j["type"] = static_cast<int>(curveType_);
    // 単位ボーダー
    j["unitBorder"] = unitValueBorder_;//
    // カーブのポイントを出力
    for(const auto& curve : curves_){
        nlohmann::json pointArray = nlohmann::json::array();
        for(const auto& p : curve){
            nlohmann::json pointJson;
            pointJson["x"] = p.x;
            pointJson["y"] = p.y;
            pointArray.push_back(pointJson);
        }
        j["curves"].push_back(pointArray);
    }
    return j;
}