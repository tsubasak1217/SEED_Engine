#pragma once
#include <BaseNode.h>

class Buttons : public BaseNode{

public:
    Buttons();
    void Draw()override;

public:

    // ピンID
    uint32_t pin_in_ID_;
    uint32_t pin_out_ID_;

    // イベントを起こすボタン一覧
    std::unordered_map<std::string,std::string>keyboardButtons_;
    std::unordered_map<std::string,std::string>padButtons_;
    std::unordered_map<std::string,std::string>mouseButtons_;

    // プルダウン用
    static const char* keyButtonNames[144];
    int32_t currentSelectKeyName_;
    static const char* padButtonNames[16];
    int32_t currentSelectPadName_;
    static const char* mouseButtonNames[3];
    int32_t currentSelectMouseName_;

private:

    void BeginNode();
    void EndNode();
};