#include "Stage.h"
#include <Game/Objects/Stage/StageManager.h>

// blocks
#include <Game/Objects/Stage/Blocks/Block_Wall.h>
#include <Game/Objects/Stage/Blocks/Block_Box.h>
#include <Game/Objects/Actor/Player/Player.h>

// SEED
#include <SEED/Source/SEED.h>

//////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
//////////////////////////////////////////////////////////////
Stage::Stage(){
}

Stage::~Stage(){
}

//////////////////////////////////////////////////////////////
// 初期化
//////////////////////////////////////////////////////////////
void Stage::Initialize(){
    Reset();
}

void Stage::Initialize(const std::string& filePath){
    Reset();
    filePath_ = filePath;
    LoadStage(filePath_);
}

//////////////////////////////////////////////////////////////
// フレームの開始時処理
//////////////////////////////////////////////////////////////
void Stage::BeginFrame(){
    for(auto& block : blocks_){
        if(block){
            block->BeginFrame();
        }
    }
}

//////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////
void Stage::Update(){
    for(auto& block : blocks_){
        if(block){
            block->Update();
        }
    }

    // ステージの状態の解決
    if(isCharacterMoved_){
        SolveStage();
        isCharacterMoved_ = false;
    }
}

//////////////////////////////////////////////////////////////
// 描画
//////////////////////////////////////////////////////////////
void Stage::Draw(){
    for(auto& block : blocks_){
        if(block){
            block->Draw();
        }
    }

#ifdef _DEBUG
    DrawStageArea();
#endif // _DEBUG

}

//////////////////////////////////////////////////////////////
// リセット
//////////////////////////////////////////////////////////////
void Stage::Reset(){
    blocks_.clear();
    ResizeStage();
}


//////////////////////////////////////////////////////////////
// マトリックスの更新
//////////////////////////////////////////////////////////////
void Stage::UpdateMatrix(){
    for(auto& block : blocks_){
        if(block){
            block->UpdateMatrix();
        }
    }
}

//////////////////////////////////////////////////////////////
// ステージの範囲を描画
//////////////////////////////////////////////////////////////
void Stage::DrawStageArea(){
    static Vector2 center;
    static Vector2 size;
    static Vector2 LT, RB;
    static float heightOffset = 0.1f;

    // ステージの範囲を計算
    center = Vector2((float)stageSize_.x - 1, (float)-(stageSize_.y - 1))
        * StageManager::GetInstance()->GetBlockSize() * 0.5f;
    size = Vector2((float)stageSize_.x, (float)-stageSize_.y)
        * StageManager::GetInstance()->GetBlockSize();

    // 左上、右下の座標を計算
    LT = center - size * 0.5f;
    RB = center + size * 0.5f;

    // マス目表示
    Vector3 LR[2] = { {LT.x,heightOffset,LT.y},{RB.x,heightOffset,LT.y} };
    Vector3 TB[2] = { {LT.x,heightOffset,LT.y},{LT.x,heightOffset,RB.y} };

    for(int32_t x = 0; x < stageSize_.y + 1; ++x){
        SEED::DrawLine(
            LR[0] + Vector3(0.0f, 0.0f, (float)x * -StageManager::GetInstance()->GetBlockSize()),
            LR[1] + Vector3(0.0f, 0.0f, (float)x * -StageManager::GetInstance()->GetBlockSize()),
            { 1.0f,1.0f,0.0f,1.0f }
        );
    }

    for(int32_t y = 0; y < stageSize_.x + 1; ++y){
        SEED::DrawLine(
            TB[0] + Vector3((float)y * StageManager::GetInstance()->GetBlockSize(), 0.0f, 0.0f),
            TB[1] + Vector3((float)y * StageManager::GetInstance()->GetBlockSize(), 0.0f, 0.0f),
            { 1.0f,1.0f,0.0f,1.0f }
        );
    }

}

//////////////////////////////////////////////////////////////
// プレイヤーが移動を要求
//////////////////////////////////////////////////////////////
bool Stage::RequestPlayerMove(const Vector2i& address, Block_Base* pBlock){

    // 範囲外の場合はfalse
    if(address.x < 0 || address.x >= stageSize_.x ||
        address.y < 0 || address.y >= stageSize_.y){
        return false;
    }

    // ブロックが存在しない場合はtrue
    if(blockMap_[address.y][address.x] == std::nullopt){
        // プレイヤーの座標を更新
        MoveBlock(pBlock->address_, address);
        return true;

    } else{

        // 壁ブロックの場合はfalse
        if(blockMap_[address.y][address.x].value().first->blockType_ != BlockType::Box){
            return false;

        } else if(blockMap_[address.y][address.x].value().first->blockType_ == BlockType::Box){
            Vector2i direction = address - pBlock->address_;
            Vector2i nextAddress = address + direction;

            // 次のアドレスが範囲外の場合はfalse
            if(nextAddress.x < 0 || nextAddress.x >= stageSize_.x ||
                nextAddress.y < 0 || nextAddress.y >= stageSize_.y){
                return false;
            }

            // 次のアドレスにブロックが存在する場合はfalse
            if(blockMap_[nextAddress.y][nextAddress.x]){
                return false;
            }

            // 1つならBoxを移動して動けるのでtrue(入れ替える)
            MoveBlock(address, nextAddress);
            // プレイヤーの座標を更新
            MoveBlock(pBlock->address_, address);

            return true;
        }

        return false;
    }
}

//////////////////////////////////////////////////////////////
// ブロックの挿入を試みる
//////////////////////////////////////////////////////////////
void Stage::TryInsertBlock(const Vector2i& address, BlockType blockType){
    // 範囲外の場合は挿入しない
    if(address.x < 0 || address.x >= stageSize_.x ||
        address.y < 0 || address.y >= stageSize_.y){
        return;
    }

    // ブロックの種類によって生成
    std::unique_ptr<Block_Base> newBlock = nullptr;
    if(blockType == BlockType::Wall){
        newBlock = std::make_unique<Block_Wall>();

    } else if(blockType == BlockType::Box){
        newBlock = std::make_unique<Block_Box>();

    } else if(blockType == BlockType::Player){

        // すでにプレイヤーが存在する場合は消してから挿入
        for(auto& block : blocks_){
            if(block && block->blockType_ == BlockType::Player){
                block.reset();
            }
        }

        newBlock = std::make_unique<Player>();
    }

    newBlock->SetAddress(address);
    newBlock->Initialize();

    // 既にブロックが存在する場合は上書き
    if(blockMap_[address.y][address.x].has_value()){
        size_t index = blockMap_[address.y][address.x].value().second;
        blocks_[index].reset();
        blocks_[index] = std::move(newBlock);
        blockMap_[address.y][address.x].value().first = blocks_[index].get();

    } else{
        // ブロックが存在しない場合は新規追加
        blocks_.emplace_back(std::move(newBlock));
        blockMap_[address.y][address.x] = std::make_pair(blocks_.back().get(), blocks_.size() - 1);
    }
}

//////////////////////////////////////////////////////////////
// ブロックの削除
//////////////////////////////////////////////////////////////
void Stage::DeleteBlock(const Vector2i& address){
    // 範囲外の場合は削除しない
    if(address.x < 0 || address.x >= stageSize_.x ||
        address.y < 0 || address.y >= stageSize_.y){
        return;
    }
    // ブロックが存在する場合は削除
    if(blockMap_[address.y][address.x].has_value()){
        
        blocks_[blockMap_[address.y][address.x].value().second].reset();
        blockMap_[address.y][address.x].reset();
    }
}

//////////////////////////////////////////////////////////////
// ステージの読み込み
//////////////////////////////////////////////////////////////
void Stage::LoadStage(const std::string& filePath){

    blocks_.clear();
    nlohmann::json json;
    std::ifstream ifs(filePath);

    if(ifs.fail()){
        std::cerr << "File do not exist." << std::endl;
        return;
    }

    ifs >> json;
    ifs.close();

    // ステージ番号の読み込み
    if(json.contains("stageNo")){
        stageNo_ = json["stageNo"];
    }

    // ステージのサイズを読み込み
    if(json.contains("stageSize")){
        stageSize_ = json["stageSize"];
    }

    // カメラの高さを読み込み
    if(json.contains("cameraHeight")){
        cameraHeight_ = json["cameraHeight"];
    }

    // ブロックの読み込み
    if(json.contains("blocks")){
        for(auto& block : json["blocks"]){
            int32_t blockType = block["type"];
            Vector2i address = block["address"];

            // ブロックの種類によって生成
            if(blockType == (int32_t)BlockType::Wall){
                blocks_.emplace_back(std::make_unique<Block_Wall>());

            } else if(blockType == (int32_t)BlockType::Box){
                blocks_.emplace_back(std::make_unique<Block_Box>());

            } else if(blockType == (int32_t)BlockType::Player){
                blocks_.emplace_back(std::make_unique<Player>());
            }

            blocks_.back()->Load(block);
            blockMap_[address.y][address.x] = std::make_pair(blocks_.back().get(), blocks_.size() - 1);
        }
    }
}

//////////////////////////////////////////////////////////////
// ステージの出力
//////////////////////////////////////////////////////////////
void Stage::OutputStage(){
    nlohmann::json j;
    j["stageNo"] = stageNo_;
    j["stageSize"] = stageSize_;
    j["cameraHeight"] = cameraHeight_;
    for(auto& block : blocks_){
        if(block){
            nlohmann::json blockJson;
            blockJson["type"] = (int32_t)block->blockType_;
            blockJson["address"] = block->address_;
            block->Output(blockJson);
            j["blocks"].push_back(blockJson);
        }
    }

    if(stageNo_ != -1){
        std::ofstream ofs("Resources/jsons/stages/stage" + std::to_string(stageNo_) + ".json");
        ofs << j.dump(4);
    } else{
        std::ofstream ofs("Resources/jsons/stages/editStage.json");
        ofs << j.dump(4);
    }

}

//////////////////////////////////////////////////////////////
// ステージのリサイズ
//////////////////////////////////////////////////////////////
void Stage::ResizeStage(){
    // ブロックの配列数を確保
    blockMap_.resize(stageSize_.y);
    for(auto& block : blockMap_){
        block.resize(stageSize_.x);
    }
}

//////////////////////////////////////////////////////////////
// ステージの状態の解決
//////////////////////////////////////////////////////////////
void Stage::SolveStage(){

    //// まず範囲外に出たブロックを元に戻す
    //for(auto& blockArray : blocks_){
    //    for(auto& block : blockArray){
    //        if(block->address_.x < 0 || block->address_.x >= stageSize_.x ||
    //            block->address_.y < 0 || block->address_.y >= stageSize_.y){
    //            block->address_ = block->preAddress_;
    //        }
    //    }
    //}

    //// 次にブロック同士の重なりを解決
    //int solveCount = 0;

    //while(true){
    //    for(int i = 0; i < blocks_.size(); ++i){
    //        for(int j = i + 1; j < blocks_.size(); ++j){
    //            if(blocks_[i]->address_ == blocks_[j]->address_){
    //                // preAddressが違う座標の場合のみ元に戻す
    //                if(blocks_[i]->address_ != blocks_[i]->preAddress_){
    //                    blocks_[i]->address_ = blocks_[i]->preAddress_;
    //                    solveCount++;
    //                }

    //                if(blocks_[j]->address_ != blocks_[j]->preAddress_){
    //                    blocks_[j]->address_ = blocks_[j]->preAddress_;
    //                    solveCount++;
    //                }
    //            }
    //        }
    //    }

    //    // 重なりがなくなったら終了
    //    if(solveCount == 0){
    //        for(auto& block : blocks_){
    //            block->preAddress_ = block->address_;
    //            block->UpdateMatrix();
    //        }
    //        break;
    //    } else{
    //        solveCount = 0;
    //    }
    //}
}

void Stage::MoveBlock(const Vector2i& from, const Vector2i& to){
    // ブロックを移動
    blockMap_[to.y][to.x] = blockMap_[from.y][from.x];
    blockMap_[from.y][from.x].reset();
    blockMap_[to.y][to.x].value().first->SetAddress(to);
    blockMap_[to.y][to.x].value().first->UpdateMatrix();
    // 移動元を空に
}

void Stage::CreateDoppelganger(const Vector2i address){

    // 回転を取得
    Vector3 rotate;
    rotate = blockMap_[address.y][address.x].value().first->GetRotate();

    // リセットする
    for(int32_t i = 0; i < blocks_.size(); i++){
        if(blocks_[i]){
            // 元居た場所をリセット
            blockMap_[blocks_[i]->address_.y][blocks_[i]->address_.x].reset();
            // アドレスを初期の場所に
            blocks_[i]->ResetAddress();
            blocks_[i]->UpdateMatrix();
            // ドッペルゲンガーがいる場所をリセット
            blockMap_[blocks_[i]->address_.y][blocks_[i]->address_.x].reset();
            blockMap_[blocks_[i]->address_.y][blocks_[i]->address_.x] = std::make_pair(blocks_[i].get(), i);
        }
    }

    // ドッペルゲンガーを生成
    auto& doppel = blocks_.emplace_back(std::make_unique<Player>());
    doppel->SetAddress(address);
    doppel->SetFirstAddress(address);
    doppel->Initialize();
    doppel->SetRotate(rotate);
    doppel->UpdateMatrix();
    doppel->SetFirstPos(doppel->GetTranslate());

    // ドッペルゲンガーがいる場所をリセット
    blockMap_[address.y][address.x].reset();
    blockMap_[address.y][address.x] = std::make_pair(doppel.get(), blocks_.size() - 1);
}
