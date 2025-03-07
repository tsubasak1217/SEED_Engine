#include <SEED/Source/SEED.h>
#include <Game/GameSystem.h>
#include <Environment/Environment.h>
#include <SEED/Source/Manager/WindowManager/WindowManager.h>

static LeakChecker leakChecker;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    SEED::Initialize(kWindowSizeX, kWindowSizeY, hInstance, nCmdShow);
    GameSystem::Initialize();

    while(WindowManager::ProcessMessage() != WM_QUIT){
        SEED::BeginFrame();
        GameSystem::Run();
        SEED::EndFrame();
    }

    GameSystem::Finalize();
    SEED::Finalize();

    return 0;
}