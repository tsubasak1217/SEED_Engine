#pragma once
#include "Scene_Base.h"
#include "State_Base.h"

// external
#include <string>
#include <vector>
#include <memory>
#include <list>

// local
#include <Triangle.h>
#include <Model.h>
#include <State_Base.h>
#include <Sprite.h>


class Scene_Game : public Scene_Base{

public:
    Scene_Game() = default;
    Scene_Game(SceneManager* pSceneManager);
    ~Scene_Game()override;
    void Initialize()override;
    void Finalize()override;
    void Update()override;
    void Draw()override;

private:

    // 解像度
    float resolutionRate_ = 1.0f;
    float preRate_ = resolutionRate_;

    // オブジェクト
    std::vector<Vector3*>controlPoints_;// 打ち込んだ座標の一覧
    std::vector<std::unique_ptr<Model>>controlModels_;// 打ち込んだ座標に表示されるモデル
    std::vector<std::unique_ptr<Model>>twistModels_;// ねじれ管理用のモデル
    const int kSubdivision_ = 16;// ベジェ曲線の一区間の分割数
};