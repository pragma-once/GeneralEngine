#include "../Engine.h"

namespace Engine
{
    namespace Utilities
    {
        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        MutexContained<SupportsSharedLock, SupportsUpgradableSharedLock>::~MutexContained() {}

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        void MutexContained<SupportsSharedLock, SupportsUpgradableSharedLock>::LockAndDo(std::function<void()> Process)
        {
            auto guard = Mutex.GetLock();
            Process();
        }

        template class MutexContained<false, false>;
        template class MutexContained<true, false>;
        template class MutexContained<true, true>;
    }
}
