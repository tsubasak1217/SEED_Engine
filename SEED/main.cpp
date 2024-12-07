#include <SEED.h>
#include <Environment.h>
#include <SceneManager.h>
#include <WindowManager.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    WindowManager::Initialize(hInstance, nCmdShow);
    SEED::Initialize(kWindowSizeX, kWindowSizeY);

    while(WindowManager::ProcessMessage() != WM_QUIT){
        SEED::BeginFrame();

        SceneManager::Update();
        SceneManager::Draw();

        SEED::EndFrame();
    }

    SEED::Finalize();
    return 0;
}