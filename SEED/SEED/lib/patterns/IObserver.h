#pragma once
#include <string>

class IObserver{
public:
    virtual ~IObserver() = default;
    virtual void OnNotify(const std::string& event, void* data = nullptr) = 0;
};