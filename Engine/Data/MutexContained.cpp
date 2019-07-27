#include "../Engine.h"

namespace Engine
{
    namespace Data
    {
        MutexContained::~MutexContained() {}

        void MutexContained::LockAndDo(std::function<void()> Process)
        {
            auto guard = Mutex.GetLock();
            Process();
        }
    }
}
