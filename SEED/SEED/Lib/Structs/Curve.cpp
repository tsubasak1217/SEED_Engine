#include "Curve.h"
#include <SEED/Lib/Functions/MyFunc/MyMath.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////
// t0~1の範囲で補間した値を取得
////////////////////////////////////////////////////////////////////////////////////////////////////////
float Curve::GetValue(float t, int channel) const{

    // 引数をクランプ
    t = std::clamp(t, 0.0f, 1.0f);
    channel = std::clamp(channel, 0, int(curves.size()) - 1);

    // チャンネルのポイントを取得
    const auto& points = curves[channel];
    if(points.size() == 0){ return 0.0f; }
    if(points.size() == 1){ return points[0].y; }

    // tが存在している区間を探す
    for(size_t i = 0; i < points.size() - 1; ++i){
        if(t >= points[i].x && t <= points[i + 1].x){
            // 区間内に存在している場合
            float localT = (t - points[i].x) / (points[i + 1].x - points[i].x);
            float ease = Easing::Ease[curveType](localT);
            return MyMath::Lerp(points[i].y, points[i + 1].y, ease);
        }
    }

    // 見つからなかった場合は0を返す
    return 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Jsonから読み込み
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Curve::FromJson(const nlohmann::json& json){
    // jsonが空の場合は何もしない
    if(json.is_null() || json.empty()){ return; }

    // チャンネル数
    if(json.contains("channel")){
        int channelCount = json["channel"].get<int>() + 1;
        curves.clear();
        curves.resize(channelCount);
    }

    // カーブタイプ
    if(json.contains("type")){
        curveType = static_cast<Easing::Type>(json["type"].get<int>());
    }

    // カーブのポイントを読み込み
    if(json.contains("curves")){
        for(int i = 0; i < curves.size(); ++i){
            if(i >= json["curves"].size()){ break; }
            curves[i].clear();
            for(const auto& pointJson : json["curves"][i]){
                if(pointJson.contains("x") && pointJson.contains("y")){
                    Vector2 p;
                    p.x = pointJson["x"].get<float>();
                    p.y = pointJson["y"].get<float>();
                    curves[i].push_back(p);
                }
            }
        }
    }
}