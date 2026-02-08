#include "SpotLight.h"
#include <SEED/Source/SEED.h>

namespace SEED{

    SpotLight::SpotLight(){
        lightType_ = SPOT_LIGHT;
        intensity = 2.0f;
    }

} // namespace SEED