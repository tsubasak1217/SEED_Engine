#pragma once
#include <unordered_map>
#include <string>
#include "DescriptorHeap.h"

class ViewManager{
public:
    ViewManager();
    ~ViewManager();

public:

    void Initialize();

private:

    std::unordered_map<std::string,std::unique_ptr<DescriptorHeap>>DescriptorHeaps_;
};