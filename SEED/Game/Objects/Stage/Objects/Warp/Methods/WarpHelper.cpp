#include "WarpHelper.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Functions/myFunc/MyMath.h>

//============================================================================
//	WarpHelper classMethods
//============================================================================

Vector4 WarpHelper::GetIndexColor(uint32_t index) {

    Vector4 color = {};
    switch (index) {
    case 0: {

        color = MyMath::FloatColor(0xff007fff);
        break;
    }
    case 1: {

        color = MyMath::FloatColor(0x1489ffff);
        break;
    }
    case 2: {

        color = MyMath::FloatColor(0x0aff84ff);
        break;
    }
    }
    return color;
}