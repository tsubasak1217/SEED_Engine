#pragma once
#include <string>
#include <cassert>

namespace SEED{
    namespace Methods{

        // Releaseビルドでも無効化されない重大エラーの記録関数
        // OutputDebugString + exeと同じディレクトリの"crash_log.txt"への追記を行う
        void LogCriticalError(const std::string& message);
    }
} // namespace SEED

#ifdef _DEBUG
// Debugでは記録した上で従来通りassertで止める(msgはstd::stringの場合もあるため素のassert(false)を使う)
#define SEED_CHECK(cond, msg) \
    do{ if(!(cond)){ SEED::Methods::LogCriticalError(msg); assert(false); } }while(false)
#define SEED_CHECK_RETURN(cond, msg, ...) \
    do{ if(!(cond)){ SEED::Methods::LogCriticalError(msg); assert(false); return __VA_ARGS__; } }while(false)
#else
// Releaseでは記録した上で安全側(早期return)に倒す
#define SEED_CHECK(cond, msg) \
    do{ if(!(cond)){ SEED::Methods::LogCriticalError(msg); } }while(false)
#define SEED_CHECK_RETURN(cond, msg, ...) \
    do{ if(!(cond)){ SEED::Methods::LogCriticalError(msg); return __VA_ARGS__; } }while(false)
#endif
