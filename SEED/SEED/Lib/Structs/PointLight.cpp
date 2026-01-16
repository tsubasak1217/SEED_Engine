#include "PointLight.h"
#include <SEED/Source/SEED.h>

namespace SEED{

    PointLight::PointLight(){
        lightType_ = POINT_LIGHT;
        intensity = 2.0f;
    }

} // namespace SEED
