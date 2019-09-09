#include "../Engine.h"

namespace Engine
{
    namespace Utilities
    {
        MutexContained::~MutexContained() {}

        void MutexContained::LockAndDo(std::function<void()> Process)
        {
            auto guard = Mutex.GetLock();
            Process();
        }
    }
}
