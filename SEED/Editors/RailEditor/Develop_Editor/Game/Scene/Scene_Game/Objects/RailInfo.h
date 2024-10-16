#pragma once
#include <vector>
#include <memory>
#include "SEED.h"

class RailInfo{

public:
    RailInfo() = default;
    ~RailInfo();

    void Update();
    void Draw();

public:
    // オブジェクト
    std::vector<Vector3*>controlPoints_;// 打ち込んだ座標の一覧
    std::vector<std::unique_ptr<Model>>controlModels_;// 打ち込んだ座標に表示されるモデル
    std::vector<std::unique_ptr<Model>>twistModels_;// ねじれ管理用のモデル
    const int kSubdivision_ = 16;// ベジェ曲線の一区間の分割数
};