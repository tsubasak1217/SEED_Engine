#include <SEED/Source/SEED.h>
#include <Game/GameSystem.h>
#include <Environment/Environment.h>
#include <SEED/Source/Manager/WindowManager/WindowManager.h>

static SEED::LeakChecker leakChecker;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{

    // エンジン側の初期化
   SEED::Instance::Initialize(kWindowSizeX, kWindowSizeY, hInstance, nCmdShow);
    // ゲーム側の初期化
    SEED::GameSystem::Initialize();

    // ゲームループ
    while(SEED::WindowManager::ProcessMessage() != WM_QUIT){
       SEED::Instance::BeginFrame();// エンジン側のフレーム開始時処理
       SEED::GameSystem::Run();// ゲーム側の1フレーム内処理
       SEED::Instance::EndFrame();// エンジン側のフレーム終了時処理
    }

    // ゲーム側の終了処理
    SEED::GameSystem::Finalize();
    // エンジン側の終了処理
   SEED::Instance::Finalize();

    return 0;
}