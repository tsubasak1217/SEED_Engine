#include <SEED/Lib/Structs/ModelAnimation.h>
#include <SEED/Lib/Functions/Math.h>

namespace SEED{

    /*--------------------------------------*/
    // 秒数からアニメーションの値を計算
    /*--------------------------------------*/
    Vector3 CalcMomentValue(const std::vector<KeyframeVec3>& keyFrames, float time){
        Vector3 result;
        // キーフレームがない場合は0を返す
        if(keyFrames.size() == 0){ return result; }
        // 最初のキーフレームより前の場合は最初のキーフレームの値を返す
        if(time <= keyFrames[0].time){ return keyFrames[0].value; }
        // 最後のキーフレームより後の場合は最後のキーフレームの値を返す
        if(time >= keyFrames[keyFrames.size() - 1].time){ return keyFrames[keyFrames.size() - 1].value; }

        // キーフレーム間の値を計算
        for(uint32_t i = 0; i < keyFrames.size(); ++i){
            if(time >= keyFrames[i].time && time <= keyFrames[i + 1].time){
                float t = (time - keyFrames[i].time) / (keyFrames[i + 1].time - keyFrames[i].time);
                result = Methods::Math::Lerp(keyFrames[i].value, keyFrames[i + 1].value, t);
                break;
            }
        }

        return result;
    }

    // クォータニオン版
    Quaternion CalcMomentValue(const std::vector<KeyframeQuaternion>& keyFrames, float time){
        Quaternion result;
        // キーフレームがない場合は0を返す
        if(keyFrames.size() == 0){ return result; }
        // 最初のキーフレームより前の場合は最初のキーフレームの値を返す
        if(time <= keyFrames[0].time){ return keyFrames[0].value; }
        // 最後のキーフレームより後の場合は最後のキーフレームの値を返す
        if(time >= keyFrames[keyFrames.size() - 1].time){ return keyFrames[keyFrames.size() - 1].value; }

        // キーフレーム間の値を計算
        for(uint32_t i = 0; i < keyFrames.size(); ++i){
            if(time >= keyFrames[i].time && time <= keyFrames[i + 1].time){
                float t = (time - keyFrames[i].time) / (keyFrames[i + 1].time - keyFrames[i].time);
                result = Quaternion::Slerp(keyFrames[i].value, keyFrames[i + 1].value, t);
                break;
            }
        }

        return result;
    }

} // namespace SEED