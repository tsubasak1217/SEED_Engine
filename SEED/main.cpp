#include <SEED/Source/SEED.h>
#include <Game/GameSystem.h>
#include <Environment/Environment.h>
#include <SEED/Source/Manager/WindowManager/WindowManager.h>

static LeakChecker leakChecker;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{

    // エンジン側の初期化
    SEED::Initialize(kWindowSizeX, kWindowSizeY, hInstance, nCmdShow);
    // ゲーム側の初期化
    GameSystem::Initialize();

    // ゲームループ
    while(WindowManager::ProcessMessage() != WM_QUIT){
        SEED::BeginFrame();// エンジン側のフレーム開始時処理
        GameSystem::Run();// ゲーム側の1フレーム内処理
        SEED::EndFrame();// エンジン側のフレーム終了時処理
    }

    // ゲーム側の終了処理
    GameSystem::Finalize();
    // エンジン側の終了処理
    SEED::Finalize();

    return 0;
}