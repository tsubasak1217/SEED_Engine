#include "BaseLight.h"
#include <SEED.h>

void BaseLight::SendData(){
    SEED::SendLightData(this);
}
