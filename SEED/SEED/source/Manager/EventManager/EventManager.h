#pragma once

// lib
#include "../../lib/patterns/IObserver.h"
#include "../../lib/patterns/ISubject.h"

// c++
#include <vector>
#include <algorithm>
#include <string>

class EventManager 
    : public ISubject{
public:
    void RegisterObserver(IObserver* observer) override{
        observers_.push_back(observer);
    }

    void UnregisterObserver(IObserver* observer) override{
        observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
    }

    void Notify(const std::string& event, void* data = nullptr) override{
        for (auto& observer : observers_){
            observer->OnNotify(event, data);
        }
    }

private:
    std::vector<IObserver*> observers_;
};
