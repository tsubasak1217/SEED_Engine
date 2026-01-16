#pragma once
#include <SEED/Source/Basic/Components/Only3D/ModelRenderComponent.h>
#include <SEED/Lib/Structs/ModelAnimation.h>

struct DragInfo_Joint{
    SEED::ModelRenderComponent* pComponent;
    SEED::ModelJoint* pJoint;
};