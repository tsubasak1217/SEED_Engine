#pragma once

#include <memory>

#include <vector>

class Egg;
class Player;

////////////////////////////////////////////////////
//  Player が 扱う 卵 を まとめて， 管理するクラス
////////////////////////////////////////////////////
// 一斉 更新，描画
// 個数 の 管理 (増やす，減らす,上限など)
class EggManager{
public:
    EggManager();
    ~EggManager();

    void Initialize();
    void Update();
    void Draw();

    void HandOverColliders();

    void BeginFrame();
    void EndFrame();

    /// <summary>
    /// 卵の数を初期化(ステージ開始時等で使用)
    /// </summary>
    void InitializeEggCount();
private:
    static const uint32_t maxEggsSize_;
    std::vector<std::unique_ptr<Egg>> eggs_;

    Player* player_ = nullptr;
public:
    Player* GetPlayer()const{ return player_; }
    void SetPlayer(Player* _player){ player_ = _player; }

    std::vector<std::unique_ptr<Egg>>& GetEggs(){ return eggs_; }

    /// <summary>
    /// 卵を増やす
    /// </summary>
    /// <param name="_egg"></param>
    void AddEgg(std::unique_ptr<Egg>& _egg);

    /// <summary>
    /// 指定した卵を削除
    /// </summary>
    /// <param name="_egg"></param>
    void RemoveEgg(Egg* _egg);

    /// <summary>
    /// 先頭 の 卵 を 入手
    /// </summary>
    /// <returns></returns>
    std::unique_ptr<Egg>& GetFrontEgg();

    std::unique_ptr<Egg>& GetBackEgg();

    /// <summary>
    /// 卵が空かどうか
    /// </summary>
    /// <returns>空なら true</returns>
    bool GetIsEmpty()const;
};