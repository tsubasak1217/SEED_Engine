#include "ResultDrawer.h"
#include <SEED/Source/SEED.h>

// リザルトを描画する関数
void ResultDrawer::DrawResult(ResultStep step, float progress){

    // 初期化されていない場合は初期化を行う
    if(!initialized){
        Initialize();
    }

    // 描画の進行状況に応じて描画処理を分岐
    switch(step){
    case ResultStep::Appear:// リザルトが表示される段階
    {
        break;
    }
    case ResultStep::CountUp:// 各種カウントを数え上げる
    {
        break;
    }
    case ResultStep::Display:// すべての結果が表示されている状態
    {
        break;
    }
    case ResultStep::Exit:// リザルトを閉じる段階
    {
        break;
    }
    default:
    {
        // 何もしない
        break;
    }
    }
}

void ResultDrawer::Initialize(){

    // 新しい項目を追加
    transformSegments.emplace("SongName");
    transformSegments.emplace("Score");
    transformSegments.emplace("Rank");
    transformSegments.emplace("Combo");
    transformSegments.emplace("Parfect");
    transformSegments.emplace("Great");
    transformSegments.emplace("Good");
    transformSegments.emplace("Miss");
    transformSegments.emplace("Fast");
    transformSegments.emplace("Late");


    initialized = true;
}
