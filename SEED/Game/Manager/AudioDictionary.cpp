#include "AudioDictionary.h"


void AudioDictionary::Initialize() {

    // ここに音声ファイルのキーとパスを追加していく
    // タイトルシーン
    dict["タイトルシーン_BGM"] = "BGM/titleBgm2.wav";
    dict["タイトル_決定"] = "SE/decide3.wav";
    // セレクトシーン
    dict["セレクトシーン_BGM"] = "BGM/selectBgm1.wav";
    dict["セレクトシーン_左右"] = "SE/decide2.wav";
    dict["セレクトシーン_決定"] = "SE/decide1.wav";
    // ゲームシーン
    dict["ゲームシーン_通常BGM"] = "BGM/TwiniwT_normal.wav";
    dict["ゲームシーン_虚像BGM"] = "BGM/TwiniwT_hologram.wav";
    // プレイヤー
    dict["プレイヤー_足音"] = "SE/playerMove.wav";
    dict["プレイヤー_死亡音"] = "SE/playerDie2.wav";
    dict["プレイヤー_ジャンプ音"] = "SE/playerJump.wav";
    dict["プレイヤー_着地音"] = "SE/playerLand.wav";
    dict["プレイヤー_ワープ音"] = "SE/warpIn.wav";
    // ミラー
    dict["ミラー_設置"] = "SE/playerPutBorder.wav";
    dict["ミラー_回収"] = "SE/playerCollectBorder.wav";
    dict["ミラー_回収不可"] = "SE/playerUnCollectBorder.wav";
    // ポーズ
    dict["ポーズ_ポーズボタン"] = "SE/pauseButton.wav";
    dict["ポーズ_選択"] = "SE/decide2.wav";
    dict["ポーズ_ゲームに戻る"] = "SE/decide1.wav";
    dict["ポーズ_リトライ"] = "SE/decide1.wav";
    dict["ポーズ_セレクトへ"] = "SE/decide1.wav";
    // ゴール、クリア
    dict["クリア_BGM"] = "BGM/clearBgm2.wav";
    dict["王冠_キラキラ音"] = "SE/reproduction.wav";
    dict["王冠_取得音"] = "SE/clearJingle1.wav";
    dict["王冠_離脱音"] = "SE/playerCollectBorder.wav";
    dict["クリアメニュー_開始"] = "SE/swoosh-06-351021.mp3";
    dict["クリアメニュー_バー"] = "SE/notification-bell-sound-1-376885.mp3";
    dict["クリアメニュー_選択"] = "SE/decide2.wav";
    dict["クリアメニュー_決定"] = "SE/decide1.wav";
}