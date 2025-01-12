#pragma once

#include "IObserver.h"
#include <string>

class ISubject{
public:
    virtual ~ISubject() = default;
    virtual void RegisterObserver(IObserver* observer) = 0;
    virtual void UnregisterObserver(IObserver* observer) = 0;
    virtual void Notify(const std::string& event, void* data = nullptr) = 0;
};