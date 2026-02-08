#pragma once
#include <SEED/Lib/Tensor/Vector3.h>

namespace SEED{

    /// <summary>
    /// GPUに転送する用の用のカメラ情報
    /// </summary>
    struct CameraForGPU{
        Vector3 position;
    };

} // namespace SEED