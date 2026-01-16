#include "BaseLight.h"
#include <SEED/Source/SEED.h>

namespace SEED{

    void BaseLight::SendData(){
        SEED::Instance::SendLightData(this);
    }

} // namespace SEED
