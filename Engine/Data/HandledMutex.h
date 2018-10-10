#pragma once

#include "../Engine.dec.h"

namespace Engine
{
    namespace Data
    {
        class HandledMutex
        {
        public:
            struct LockGuard
            {
                friend HandledMutex;
            public:
                LockGuard& operator=(const LockGuard&);
                LockGuard();
                ~LockGuard();
            private:
                LockGuard(HandledMutex * m);
                HandledMutex * m;
            };

            struct SharedLockGuard
            {
                friend HandledMutex;
            public:
                SharedLockGuard& operator=(const SharedLockGuard&);
                SharedLockGuard();
                ~SharedLockGuard();
            private:
                SharedLockGuard(HandledMutex * m);
                HandledMutex * m;
            };

            HandledMutex();
            bool Lock();
            bool TryLock();
            bool Unlock();
            LockGuard GetLock();
            bool TryGetLock(LockGuard &GuardOut);
            bool LockShared();
            bool TryLockShared();
            bool UnlockShared();
            SharedLockGuard GetSharedLock();
            bool TryGetSharedLock(SharedLockGuard &GuardOut);
        private:
            std::mutex OwnerMutex;
            std::shared_mutex Mutex;
            bool HasOwner;
            std::thread::id Owner;
            Collections::List<std::thread::id, false> * SharedOwners;
        };
    }
}