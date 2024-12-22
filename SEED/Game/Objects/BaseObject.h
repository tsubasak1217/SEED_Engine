#pragma once
#include "Model.h"
#include <memory>
#include <cstdint>
#include <string>

class BaseObject{
public:
    BaseObject();
    virtual ~BaseObject();
    virtual void update();
    virtual void render();

private:
    static uint32_t nextID_;
    uint32_t objectID_;
    std::string name_;

private:
    std::unique_ptr<Model> model_;
};