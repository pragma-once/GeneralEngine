#pragma once

#include "../Engine.dec.h"

namespace Engine
{
    namespace Utilities
    {
        class SmartMutex final
        {
        public:
            /// @brief Unlocks a SmartMutex lock on destruction.
            class LockGuard final
            {
                friend SmartMutex;
            public:
                LockGuard();
                LockGuard(const LockGuard&);
                LockGuard(LockGuard&&);
                LockGuard& operator=(const LockGuard&);
                LockGuard& operator=(LockGuard&&);
                /// @brief Unlocks the guard manually.
                void Unlock();
                ~LockGuard();
            private:
                LockGuard(SmartMutex * m);
                SmartMutex * m;
            };

            /// @brief Unlocks a SmartMutex shared lock on destruction.
            class SharedLockGuard final
            {
                friend SmartMutex;
            public:
                SharedLockGuard();
                SharedLockGuard(const SharedLockGuard&);
                SharedLockGuard(SharedLockGuard&&);
                SharedLockGuard& operator=(const SharedLockGuard&);
                SharedLockGuard& operator=(SharedLockGuard&&);
                /// @brief Unlocks the guard manually.
                void Unlock();
                ~SharedLockGuard();
            private:
                SharedLockGuard(SmartMutex * m);
                SmartMutex * m;
            };

            class DeadlockException : public std::runtime_error
            {
                friend SmartMutex;
            private:
                DeadlockException();
            };

            class PossibleLivelockException : public std::runtime_error
            {
                friend SmartMutex;
            private:
                PossibleLivelockException();
            };

            friend LockGuard;
            friend SharedLockGuard;

            SmartMutex();
            ~SmartMutex();

            /// @brief Locks the mutex and returns the lock guard.
            ///
            /// Avoid shared-locking and then locking,
            /// doing so on multiple threads may result in a deadlock exception.
            ///
            /// The mutex will lock anyway if it was shared-locked by this thread only.
            LockGuard GetLock();
            /// @brief Tries to lock the mutex without waiting
            ///        and sets the GuardOut parameter if successful.
            ///
            /// Avoid shared-locking and then trying to lock,
            /// doing so on multiple threads may result in a livelock, with no thrown exception.
            ///
            /// @param GuardOut The lock guard if any.
            /// @return Whether the mutex was not locked or shared-locked by another thread
            ///         in which case the lock is successful.
            bool TryGetLock(LockGuard &GuardOut);

            /// @brief Shared-locks the mutex and returns the lock guard.
            ///
            /// Avoid shared-locking and then locking,
            /// doing so using GetLock on multiple threads may result in a deadlock exception
            /// or may result in a livelock with no thrown exception when using TryGetLock.
            ///
            /// Shared-lock is used for reads. Multiple threads can shared-lock but
            /// no other thread can lock the mutex for writes while it is shared-locked.
            SharedLockGuard GetSharedLock();
            /// @brief Tries to shared-lock the mutex without waiting
            ///        and sets the GuardOut parameter if successful.
            ///
            /// Avoid shared-locking and then locking,
            /// doing so using GetLock on multiple threads may result in a deadlock exception
            /// or may result in a livelock with no thrown exception when using TryGetLock.
            ///
            /// Shared-lock is used for reads. Multiple threads can shared-lock but
            /// no other thread can lock the mutex for writes while it is shared-locked.
            ///
            /// @param GuardOut The lock guard if any.
            /// @return Whether the mutex was not locked by another thread
            ///         in which case the shared-lock is successful.
            bool TryGetSharedLock(SharedLockGuard &GuardOut);
        private:
            std::mutex StateMutex;
            std::condition_variable ConditionVariable;

            bool HasOwner;
            std::thread::id Owner;
            int LockGuardCount;

            // Values: GuardCount
            Collections::Dictionary<std::thread::id, int, false> * SharedOwnersRef;

            bool RequestingToLockWhileSharedLocked;

            bool LockOperation(std::unique_lock<std::mutex>&);
            bool UnlockOperation(std::unique_lock<std::mutex>&);
            bool SharedLockOperation(std::unique_lock<std::mutex>&);
            bool SharedUnlockOperation(std::unique_lock<std::mutex>&);

            void LockByGuard();
            void UnlockByGuard();
            void SharedLockByGuard();
            void SharedUnlockByGuard();

            enum TryResult : std::int_fast8_t
            {
                LockedByOtherThreads = 0,
                LockedByThisThread = -1,
                LockSuccessful = 1
            };

            TryResult TryLock();
            TryResult TrySharedLock();
        };
    }
}
