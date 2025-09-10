#include "ParticleMachine.h"
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Functions/MyFunc/MyFunc.h>
#include <SEED/Lib/Functions/MyFunc/MatrixFunc.h>

void ParticleMachine2D::Draw(){

    for(auto& emitter : emitters_){
        emitter;
    }
}

void ParticleMachine2D::Emit(const Emitter2D& emitter){

    if(!emitter.emitTimer.IsFinished()){
        return;
    }

    for(int i = 0; i < emitter.numEmitEvery; ++i){
        Particle2D* particle = new Particle2D();

        // ランダムな位置に生成
        Range2D emitArea = {
            emitter.position - emitter.emitRange * 0.5f,
            emitter.position + emitter.emitRange * 0.5f
        };
        Vector2 randomPos = MyFunc::Random(emitArea);
        particle->transform.translate = randomPos;

        // ランダムな速度
        float randomAngle = MyFunc::Random(0.0f, std::numbers::pi_v<float> *2.0f);
        float randomSpeed = MyFunc::Random(emitter.speedRange);
        float randDirectionRot = MyFunc::Random(
            -std::numbers::pi_v<float> *emitter.directionRange,
            std::numbers::pi_v<float> *emitter.directionRange
        );
        Vector2 direction = emitter.baseDirection * RotateMatrix(randDirectionRot);

        particle->transform.rotate = randomAngle;
        particle->direction = direction;
        particle->speed = randomSpeed;
        particle->lifeTimer = Timer(MyFunc::Random(emitter.lifeTimeRange));



    }

}
