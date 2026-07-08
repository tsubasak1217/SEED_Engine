#include "UserManager.h"
#include <SEED/Source/Manager/HttpManager/HttpManager.h>
#include <fstream>
#include <json.hpp>
#include <Windows.h>

#define ID_BTN_LOGIN 101
#define ID_BTN_REGISTER 102

UserManager* UserManager::instance_ = nullptr;

UserManager* UserManager::GetInstance() {
    if (!instance_) {
        instance_ = new UserManager();
    }
    return instance_;
}

void UserManager::Initialize() {
    GetInstance()->Load();
}

void UserManager::Update() {
    if (!isLoggedIn_ && hLoginWnd_ == NULL) {
        OpenLoginWindow();
    }

    if (hLoginWnd_ != NULL) {
        MSG msg;
        while (PeekMessage(&msg, hLoginWnd_, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        if (hStatusText_) {
            SetWindowTextA(hStatusText_, statusMessage_.c_str());
        }
    }
}

void UserManager::OpenLoginWindow() {
    if (hLoginWnd_) return;

    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASSEXA wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "LoginWindow";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassExA(&wc);

    hLoginWnd_ = CreateWindowExA(
        0, "LoginWindow", "User Login / Registration",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hLoginWnd_, SW_SHOW);
    UpdateWindow(hLoginWnd_);
}

void UserManager::CreateControls(HWND hwnd) {
    CreateWindowExA(0, "STATIC", "Username:", WS_CHILD | WS_VISIBLE, 20, 20, 100, 20, hwnd, NULL, NULL, NULL);
    hEditName_ = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", userName_.c_str(), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 130, 20, 230, 25, hwnd, NULL, NULL, NULL);

    CreateWindowExA(0, "STATIC", "Password:", WS_CHILD | WS_VISIBLE, 20, 60, 100, 20, hwnd, NULL, NULL, NULL);
    hEditPass_ = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_PASSWORD | ES_AUTOHSCROLL, 130, 60, 230, 25, hwnd, NULL, NULL, NULL);

    hBtnLogin_ = CreateWindowExA(0, "BUTTON", "Login/Switch", WS_CHILD | WS_VISIBLE, 130, 100, 110, 30, hwnd, (HMENU)ID_BTN_LOGIN, NULL, NULL);
    hBtnRegister_ = CreateWindowExA(0, "BUTTON", "Register", WS_CHILD | WS_VISIBLE, 250, 100, 110, 30, hwnd, (HMENU)ID_BTN_REGISTER, NULL, NULL);

    hStatusText_ = CreateWindowExA(0, "STATIC", "", WS_CHILD | WS_VISIBLE, 20, 150, 340, 60, hwnd, NULL, NULL, NULL);
}

LRESULT CALLBACK UserManager::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    UserManager* inst = GetInstance();
    switch (msg) {
    case WM_CREATE:
        inst->CreateControls(hwnd);
        break;
    case WM_COMMAND:
        if (LOWORD(wp) == ID_BTN_LOGIN) {
            char name[256], pass[256];
            GetWindowTextA(inst->hEditName_, name, 256);
            GetWindowTextA(inst->hEditPass_, pass, 256);
            inst->Login(name, pass);
        } else if (LOWORD(wp) == ID_BTN_REGISTER) {
            char name[256], pass[256];
            GetWindowTextA(inst->hEditName_, name, 256);
            GetWindowTextA(inst->hEditPass_, pass, 256);
            inst->Register(name, pass);
        }
        break;
    case WM_DESTROY:
        inst->hLoginWnd_ = NULL;
        break;
    default:
        return DefWindowProcA(hwnd, msg, wp, lp);
    }
    return 0;
}

void UserManager::Login(const std::string& name, const std::string& password) {
    if (isProcessing_) return;
    if (name.empty() || password.empty()) {
        statusMessage_ = "Name and Password are required.";
        return;
    }
    isProcessing_ = true;
    statusMessage_ = "Logging in...";

    nlohmann::json j;
    j["name"] = name;
    j["password"] = password;

    SEED::HttpManager::RequestAsync("POST", "http://localhost:3000/user/login", j.dump(), [this, name](bool success, const std::string& body) {
        isProcessing_ = false;
        if (!success) {
            statusMessage_ = "Connection failed.";
            return;
        }

        try {
            auto resp = nlohmann::json::parse(body);
            if (resp.contains("token")) {
                token_ = resp["token"];
                userName_ = name;
                isLoggedIn_ = true;
                statusMessage_ = "Login successful!";
                Save();
                if (hLoginWnd_) DestroyWindow(hLoginWnd_);
            } else if (resp.contains("error")) {
                statusMessage_ = resp["error"];
            } else {
                statusMessage_ = "Login failed.";
            }
        } catch (...) {
            statusMessage_ = "Invalid response from server.";
        }
    });
}

void UserManager::Register(const std::string& name, const std::string& password) {
    if (isProcessing_) return;
    if (name.empty() || password.empty()) {
        statusMessage_ = "Name and Password are required.";
        return;
    }
    isProcessing_ = true;
    statusMessage_ = "Registering...";

    nlohmann::json j;
    j["name"] = name;
    j["password"] = password;

    SEED::HttpManager::RequestAsync("POST", "http://localhost:3000/users/new", j.dump(), [this, name, password](bool success, const std::string& body) {
        isProcessing_ = false;
        if (!success) {
            statusMessage_ = "Connection failed.";
            return;
        }

        try {
            auto resp = nlohmann::json::parse(body);
            if (resp.contains("id")) {
                statusMessage_ = "Registration successful! Logging in...";
                Login(name, password);
            } else if (resp.contains("error")) {
                statusMessage_ = resp["error"];
            } else {
                statusMessage_ = "Registration failed.";
            }
        } catch (...) {
            statusMessage_ = "Invalid response from server.";
        }
    });
}

void UserManager::Logout() {
    isLoggedIn_ = false;
    userName_ = "";
    token_ = "";
    statusMessage_ = "Logged out.";
    Save();
}

void UserManager::Save() {
    nlohmann::json j;
    j["isLoggedIn"] = isLoggedIn_;
    j["userName"] = userName_;
    j["token"] = token_;

    std::ofstream file("user_data.json");
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
    }
}

bool UserManager::Load() {
    std::ifstream file("user_data.json");
    if (!file.is_open()) return false;

    try {
        nlohmann::json j;
        file >> j;
        isLoggedIn_ = j.value("isLoggedIn", false);
        userName_ = j.value("userName", "");
        token_ = j.value("token", "");
        file.close();
        return true;
    } catch (...) {
        file.close();
        return false;
    }
}
