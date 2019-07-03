#pragma once

#include "../Engine.dec.h"

namespace Engine
{
    namespace Data
    {
        class HandledMutex final
        {
        public:
            /// @brief Unlocks a HandledMutex lock on destruction.
            class LockGuard final
            {
                friend HandledMutex;
            public:
                LockGuard();
                LockGuard(LockGuard&);
                LockGuard(LockGuard&&);
                LockGuard& operator=(LockGuard&);
                LockGuard& operator=(LockGuard&&);
                /// @brief Unlocks the guard manually.
                void Unlock();
                ~LockGuard();
            private:
                LockGuard(HandledMutex * m);
                HandledMutex * m;
            };

            /// @brief Unlocks a HandledMutex shared lock on destruction.
            class SharedLockGuard final
            {
                friend HandledMutex;
            public:
                SharedLockGuard();
                SharedLockGuard(SharedLockGuard&);
                SharedLockGuard(SharedLockGuard&&);
                SharedLockGuard& operator=(SharedLockGuard&);
                SharedLockGuard& operator=(SharedLockGuard&&);
                /// @brief Unlocks the guard manually.
                void Unlock();
                ~SharedLockGuard();
            private:
                SharedLockGuard(HandledMutex * m);
                HandledMutex * m;
            };

            friend LockGuard;
            friend SharedLockGuard;

            HandledMutex();
            ~HandledMutex();
            
            /// @brief Locks the mutex manually.
            ///
            /// It's recommended to use GetLock instead.
            /// The mutex will lock anyway if it was shared-locked by this thread only.
            /// Only locks if the mutex was not locked in the same thread.
            ///
            /// @return Whether the mutex was not locked in the same thread
            ///         and is locked by this call.
            bool Lock();
            /// @brief Tries to lock the mutex manually.
            ///
            /// It's recommended to use TryGetLock instead.
            ///
            /// @return Whether the mutex was not locked and is locked by this call.
            bool TryLock();
            /// @brief Unlocks the mutex manually.
            ///
            /// It's recommended to use the lock guards returned by GetLock or TryGetLock instead.
            /// The mutex will shared-lock if it was shared-locked by this thread alone
            /// before locking.
            ///
            /// @return Whether the mutex was locked by this thread
            ///         and is unlocked by this call.
            bool Unlock();
            /// @brief Locks the mutex and returns the lock guard.
            ///
            /// The mutex will lock anyway if it was shared-locked by this thread only.
            /// Only locks if the mutex was not locked in the same thread.
            LockGuard GetLock();
            /// @brief Tries to lock the mutex and gives the lock guard if locked.
            /// @param GuardOut The lock guard if any.
            /// @return Whether the mutex was not locked and is locked by this call.
            bool TryGetLock(LockGuard &GuardOut);

            /// @brief Shared-locks the mutex manually.
            ///
            /// It's recommended to use GetSharedLock instead.
            /// Only shared-locks if the mutex was not shared-locked in the same thread.
            ///
            /// Shared-lock is used for reads. Multiple threads can shared-lock but
            /// no other thread can lock the mutex for writes while it is shared-locked.
            ///
            /// @return Whether the mutex was not locked, or shared-locked in the same thread
            ///         and is shared-locked by this call.
            bool LockShared();
            /// @brief Tries to shared-lock the mutex manually.
            ///
            /// It's recommended to use TryGetSharedLock instead.
            ///
            /// Shared-lock is used for reads. Multiple threads can shared-lock but
            /// no other thread can lock the mutex for writes while it is shared-locked.
            ///
            /// @return Whether the mutex was not locked, or shared-locked by this thread
            ///         and is shared-locked by this call.
            bool TryLockShared();
            /// @brief Shared-unlocks the mutex manually.
            ///
            /// It's recommended to use the lock guards returned by GetSharedLock or TryGetSharedLock instead.
            ///
            /// Shared-lock is used for reads. Multiple threads can shared-lock but
            /// no other thread can lock the mutex for writes while it is shared-locked.
            ///
            /// @return Whether the mutex was shared-locked by this thread
            ///         and is unlocked by this call.
            bool UnlockShared();
            /// @brief Shared-locks the mutex and returns the lock guard.
            ///
            /// Only shared-locks if the mutex was not shared-locked in the same thread.
            ///
            /// Shared-lock is used for reads. Multiple threads can shared-lock but
            /// no other thread can lock the mutex for writes while it is shared-locked.
            SharedLockGuard GetSharedLock();
            /// @brief Tries to shared-lock the mutex and gives the lock guard if shared-locked.
            /// @param GuardOut The lock guard if any.
            /// @return Whether the mutex was not locked, or shared-locked by this thread
            ///         and is shared-locked by this call.
            bool TryGetSharedLock(SharedLockGuard &GuardOut);
        private:
            std::mutex MembersMutex;
            std::shared_mutex Mutex;
            bool HasOwner;
            std::thread::id Owner;
            Collections::Dictionary<std::thread::id, int, false> * SharedOwnersRef;
            int LockGuardCount;

            bool LockOperation(std::unique_lock<std::mutex>&);
            bool UnlockOperation();
            bool LockSharedOperation(std::unique_lock<std::mutex>&);
            bool UnlockSharedOperation();

            void LockByGuard();
            void UnlockByGuard();
            void LockSharedByGuard();
            void UnlockSharedByGuard();
        };
    }
}
