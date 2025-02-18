#include "BaseLight.h"
#include <SEED/Source/SEED.h>

void BaseLight::SendData(){
    SEED::SendLightData(this);
}
