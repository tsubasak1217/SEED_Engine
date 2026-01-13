#include "BaseLight.h"
#include <SEED/Source/SEED.h>

void BaseLight::SendData(){
   SEED::Instance::SendLightData(this);
}
