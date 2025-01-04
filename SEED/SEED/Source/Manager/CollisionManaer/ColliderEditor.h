#pragma once
#include <vector>
#include <json.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include "ImGuiManager.h"
#include "Vector3.h"
#include "Collision/Collider.h"

class ColliderEditor{
public:
    ColliderEditor();
    ~ColliderEditor();

public:
    void Edit();

private:
    std::vector<std::vector<std::unique_ptr<Collider>>>colliders_;

};