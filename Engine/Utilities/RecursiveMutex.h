#pragma once

#include "../Engine.dec.h"

namespace Engine
{
    namespace Utilities
    {
        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        class RecursiveMutex final
        {
            static_assert(
                SupportsSharedLock || !SupportsUpgradableSharedLock,
                "The mutex must support SharedLock in order to support UpgradableSharedLock."
            );
        public:
            /// @brief Unlocks a RecursiveMutex lock on destruction.
            class LockGuard final
            {
                friend RecursiveMutex;
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
                LockGuard(RecursiveMutex * m);
                RecursiveMutex * m;
            };

            /// @brief Unlocks a RecursiveMutex shared lock on destruction.
            class SharedLockGuard final
            {
                friend RecursiveMutex;
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
                SharedLockGuard(RecursiveMutex * m);
                RecursiveMutex * m;
            };

            /// @brief Unlocks a RecursiveMutex upgradable shared lock on destruction.
            class UpgradableSharedLockGuard final
            {
                friend RecursiveMutex;
            public:
                UpgradableSharedLockGuard();
                UpgradableSharedLockGuard(const UpgradableSharedLockGuard&);
                UpgradableSharedLockGuard(UpgradableSharedLockGuard&&);
                UpgradableSharedLockGuard& operator=(const UpgradableSharedLockGuard&);
                UpgradableSharedLockGuard& operator=(UpgradableSharedLockGuard&&);
                /// @brief Unlocks the guard manually.
                void Unlock();
                ~UpgradableSharedLockGuard();
            private:
                UpgradableSharedLockGuard(RecursiveMutex * m);
                RecursiveMutex * m;
            };

            friend LockGuard;
            friend SharedLockGuard;
            friend UpgradableSharedLockGuard;

            // TODO: Same exception classes for all template parameters

            class InvalidOperation : public std::runtime_error
            {
                friend RecursiveMutex;
            private:
                InvalidOperation(const char*);
            };

            class DeadlockException : public InvalidOperation
            {
                friend RecursiveMutex;
            private:
                DeadlockException();
            };

            class PossibleLivelockException : public InvalidOperation
            {
                friend RecursiveMutex;
            private:
                PossibleLivelockException();
            };

            class UpgradableSharedLockAfterSharedLockException : public InvalidOperation
            {
                friend RecursiveMutex;
            private:
                UpgradableSharedLockAfterSharedLockException();
            };

            RecursiveMutex();
            ~RecursiveMutex();

            /// @brief Locks the mutex and returns the lock guard.
            ///
            /// Avoid shared-locking and then locking,
            /// doing so on multiple threads results in a deadlock exception.
            /// To do so, use GetUpgradableSharedLock instead. (if supported by the type)
            ///
            /// The mutex will lock anyway if it was shared-locked by this thread only.
            LockGuard GetLock();
            /// @brief Tries to lock the mutex without waiting
            ///        and sets the GuardOut parameter if successful.
            ///
            /// Avoid shared-locking and then trying to lock,
            /// doing so on multiple threads results in a possible-livelock exception.
            /// To do so, use GetUpgradableSharedLock instead. (if supported by the type)
            ///
            /// @param GuardOut The lock guard if any.
            /// @return Whether the mutex was not locked or shared-locked by another thread
            ///         in which case the lock is successful.
            bool TryGetLock(LockGuard& GuardOut);

            /// @brief Shared-locks the mutex and returns the lock guard.
            ///
            /// Avoid shared-locking and then locking,
            /// doing so using GetLock on multiple threads results in a deadlock exception
            /// or a possible-livelock exception when using TryGetLock.
            /// To do so, use GetUpgradableSharedLock instead. (if supported by the type)
            ///
            /// Shared-lock is used for reads. Multiple threads can shared-lock but
            /// no other thread can lock the mutex for writes while it is shared-locked.
            template <bool Dummy = SupportsSharedLock> // So that this is not defined by default
            constexpr SharedLockGuard GetSharedLock()
            {
                static_assert(SupportsSharedLock, "Shared-lock is not supported for this type.");
                return _GetSharedLock();
            }
            /// @brief Tries to shared-lock the mutex without waiting
            ///        and sets the GuardOut parameter if successful.
            ///
            /// Avoid shared-locking and then locking,
            /// doing so using GetLock on multiple threads results in a deadlock exception
            /// or a possible-livelock exception when using TryGetLock.
            /// To do so, use GetUpgradableSharedLock instead. (if supported by the type)
            ///
            /// Shared-lock is used for reads. Multiple threads can shared-lock but
            /// no other thread can lock the mutex for writes while it is shared-locked.
            ///
            /// @param GuardOut The lock guard if any.
            /// @return Whether the mutex was not locked by another thread
            ///         in which case the shared-lock is successful.
            template <bool Dummy = SupportsSharedLock> // So that this is not defined by default
            constexpr bool TryGetSharedLock(SharedLockGuard& GuardOut)
            {
                static_assert(SupportsSharedLock, "Shared-lock is not supported for this type.");
                return _TryGetSharedLock(GuardOut);
            }

            /// @brief Acquires upgradable-shared-lock on the mutex and returns the lock guard.
            ///
            /// Upgradable-shared-lock is used for reads that may later be upgraded to write.
            /// Multiple threads can shared-lock, only 1 thread can have upgradable-shared-lock but
            /// no other thread can lock the mutex for writes while it is shared-locked.
            template <bool Dummy = SupportsUpgradableSharedLock> // So that this is not defined by default
            constexpr UpgradableSharedLockGuard GetUpgradableSharedLock()
            {
                static_assert(SupportsUpgradableSharedLock, "Upgradable-shared-lock is not supported for this type.");
                return _GetUpgradableSharedLock();
            }
            /// @brief Tries to acquire upgradable-shared-lock on the mutex without waiting
            ///        and sets the GuardOut parameter if successful.
            ///
            /// Upgradable-shared-lock is used for reads that may later be upgraded to write.
            /// Multiple threads can shared-lock, only 1 thread can have upgradable-shared-lock but
            /// no other thread can lock the mutex for writes while it is shared-locked.
            ///
            /// @param GuardOut The lock guard if any.
            /// @return Whether the mutex was not locked nor upgradable-shared-locked by another thread
            ///         in which case the upgradable-shared-lock is acquired successfully.
            template <bool Dummy = SupportsUpgradableSharedLock> // So that this is not defined by default
            constexpr bool TryGetUpgradableSharedLock(UpgradableSharedLockGuard& GuardOut)
            {
                static_assert(SupportsUpgradableSharedLock, "Upgradable-shared-lock is not supported for this type.");
                return _TryGetUpgradableSharedLock(GuardOut);
            }
        private:
            SharedLockGuard _GetSharedLock();
            bool _TryGetSharedLock(SharedLockGuard& GuardOut);
            UpgradableSharedLockGuard _GetUpgradableSharedLock();
            bool _TryGetUpgradableSharedLock(UpgradableSharedLockGuard& GuardOut);

            std::mutex StateMutex;
            std::condition_variable ConditionVariable;

            class Empty {};

            // Lock variables

            bool HasOwner;
            std::thread::id Owner;
            int LockGuardCount;

            // SharedLock variables

            // Values: GuardCount
            typename std::conditional<
                SupportsSharedLock,
                Collections::Dictionary<std::thread::id, int, false>*,
                Empty
            >::type SharedOwnersRef;

            // UpgradableSharedLock variables

            typename std::conditional<SupportsUpgradableSharedLock, bool,            Empty>::type HasUpgradableOwner;
            typename std::conditional<SupportsUpgradableSharedLock, std::thread::id, Empty>::type UpgradableOwner;
            typename std::conditional<SupportsUpgradableSharedLock, int,             Empty>::type UpgradableLockGuardCount;

            bool LockOperation(std::unique_lock<std::mutex>&);
            bool UnlockOperation(std::unique_lock<std::mutex>&);
            bool SharedLockOperation(std::unique_lock<std::mutex>&);
            bool SharedUnlockOperation(std::unique_lock<std::mutex>&);
            bool UpgradableSharedLockOperation(std::unique_lock<std::mutex>&);
            bool UpgradableSharedUnlockOperation(std::unique_lock<std::mutex>&);

            void LockByGuard();
            void UnlockByGuard();
            void SharedLockByGuard();
            void SharedUnlockByGuard();
            void UpgradableSharedLockByGuard();
            void UpgradableSharedUnlockByGuard();

            enum TryResult : std::int_fast8_t
            {
                LockedByOtherThreads = 0,
                LockedByThisThread = -1,
                LockSuccessful = 1
            };

            TryResult TryLock();
            TryResult TrySharedLock();
            TryResult TryUpgradableSharedLock();
        };
    }
}
