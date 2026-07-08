#pragma once
#include <string>
#include <json.hpp>
#include <Windows.h>

class UserManager {
private:
    UserManager() = default;
    UserManager(const UserManager&) = delete;
    UserManager& operator=(const UserManager&) = delete;
    static UserManager* instance_;

public:
    static UserManager* GetInstance();
    static void Initialize();
    
    void Update();
    
    bool IsLoggedIn() const { return isLoggedIn_; }
    const std::string& GetUserName() const { return userName_; }
    const std::string& GetToken() const { return token_; }
    void SetStatusMessage(const std::string& msg) { statusMessage_ = msg; }

    void Login(const std::string& name, const std::string& password);
    void Register(const std::string& name, const std::string& password);
    void Logout();

    void Save();
    bool Load();

    void OpenLoginWindow();

    HWND hLoginWnd_ = NULL;

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
    void CreateControls(HWND hwnd);

    bool isLoggedIn_ = false;
    std::string userName_;
    std::string token_;
    
    HWND hEditName_ = NULL;
    HWND hEditPass_ = NULL;
    HWND hBtnLogin_ = NULL;
    HWND hBtnRegister_ = NULL;
    HWND hStatusText_ = NULL;

    bool isProcessing_ = false;
    std::string statusMessage_;
};
