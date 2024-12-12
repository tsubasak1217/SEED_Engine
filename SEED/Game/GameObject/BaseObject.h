#pragma once

#include <memory>

#include <string>

class Model;

class BaseObject{
public:
    BaseObject();
    BaseObject(const std::string& _name);
    ~BaseObject();

    virtual void Initialize() = 0;
    virtual void Update() = 0;
    void Draw();

protected:
    std::unique_ptr<Model> model_;
private:
    std::string name_ = "";
public:
    const std::string& GetName()const{ return name_; }
    void SetName(const std::string& _name){ name_ = _name.c_str(); }
};