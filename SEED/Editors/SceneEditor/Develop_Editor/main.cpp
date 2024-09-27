#include <SEED.h>
#include <Environment.h>
#include <SceneManager.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    SEED::Initialize(hInstance, nCmdShow, kWindowTitle, 1280, 720);

    while(SEED::ProcessMessage() != WM_QUIT){
        SEED::BeginFrame();

        SceneManager::Update();
        SceneManager::Draw();

        SEED::EndFrame();
    }

    SEED::Finalize();
    return 0;
}