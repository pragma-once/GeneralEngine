#include "../Engine.h"

namespace Engine
{
    namespace Utilities
    {

// -------- LOCK GUARD -------- //

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::LockGuard::LockGuard()
            : m(nullptr) {}

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::LockGuard::LockGuard(const LockGuard& op)
        {
            m = op.m;
            if (m != nullptr)
                m->LockByGuard();
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::LockGuard::LockGuard(LockGuard&& op)
        {
            m = std::exchange(op.m, nullptr);
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        typename RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::LockGuard&
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::LockGuard::operator=(const LockGuard& op)
        {
            if (m != nullptr)
                m->UnlockByGuard();
            m = op.m;
            if (m != nullptr)
                m->LockByGuard();
            return *this;
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        typename RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::LockGuard&
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::LockGuard::operator=(LockGuard&& op)
        {
            if (m != nullptr)
                m->UnlockByGuard();
            m = std::exchange(op.m, nullptr);
            return *this;
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        void RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::LockGuard::Unlock()
        {
            if (m != nullptr)
            {
                m->UnlockByGuard();
                m = nullptr;
            }
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::LockGuard::~LockGuard()
        {
            if (m != nullptr)
                m->UnlockByGuard();
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::LockGuard::LockGuard(RecursiveMutex * m)
            : m(m)
        {
            if (m != nullptr)
                m->LockByGuard();
        }

// -------- SHARED-LOCK GUARD -------- //

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::SharedLockGuard::SharedLockGuard()
            : m(nullptr) {}

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::SharedLockGuard::SharedLockGuard(const SharedLockGuard& op)
        {
            m = op.m;
            if (m != nullptr)
                m->SharedLockByGuard();
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::SharedLockGuard::SharedLockGuard(SharedLockGuard&& op)
        {
            m = std::exchange(op.m, nullptr);
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        typename RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::SharedLockGuard&
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::SharedLockGuard::operator=(const SharedLockGuard& op)
        {
            if (m != nullptr)
                m->SharedUnlockByGuard();
            m = op.m;
            if (m != nullptr)
                m->SharedLockByGuard();
            return *this;
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        typename RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::SharedLockGuard&
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::SharedLockGuard::operator=(SharedLockGuard&& op)
        {
            if (m != nullptr)
                m->SharedUnlockByGuard();
            m = std::exchange(op.m, nullptr);
            return *this;
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        void RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::SharedLockGuard::Unlock()
        {
            if (m != nullptr)
            {
                m->SharedUnlockByGuard();
                m = nullptr;
            }
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::SharedLockGuard::~SharedLockGuard()
        {
            if (m != nullptr)
                m->SharedUnlockByGuard();
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::SharedLockGuard::SharedLockGuard(RecursiveMutex * m)
            : m(m)
        {
            if (m != nullptr)
                m->SharedLockByGuard();
        }

// -------- UPGRADABLE-SHARED-LOCK GUARD -------- //

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::UpgradableSharedLockGuard::UpgradableSharedLockGuard()
            : m(nullptr) {}

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::UpgradableSharedLockGuard::UpgradableSharedLockGuard(const UpgradableSharedLockGuard& op)
        {
            m = op.m;
            if (m != nullptr)
                m->UpgradableSharedLockByGuard();
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::UpgradableSharedLockGuard::UpgradableSharedLockGuard(UpgradableSharedLockGuard&& op)
        {
            m = std::exchange(op.m, nullptr);
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        typename RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::UpgradableSharedLockGuard&
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::UpgradableSharedLockGuard::operator=(const UpgradableSharedLockGuard& op)
        {
            if (m != nullptr)
                m->UpgradableSharedUnlockByGuard();
            m = op.m;
            if (m != nullptr)
                m->UpgradableSharedLockByGuard();
            return *this;
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        typename RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::UpgradableSharedLockGuard&
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::UpgradableSharedLockGuard::operator=(UpgradableSharedLockGuard&& op)
        {
            if (m != nullptr)
                m->UpgradableSharedUnlockByGuard();
            m = std::exchange(op.m, nullptr);
            return *this;
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        void RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::UpgradableSharedLockGuard::Unlock()
        {
            if (m != nullptr)
            {
                m->UpgradableSharedUnlockByGuard();
                m = nullptr;
            }
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::UpgradableSharedLockGuard::~UpgradableSharedLockGuard()
        {
            if (m != nullptr)
                m->UpgradableSharedUnlockByGuard();
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::UpgradableSharedLockGuard::UpgradableSharedLockGuard(RecursiveMutex * m)
            : m(m)
        {
            if (m != nullptr)
                m->UpgradableSharedLockByGuard();
        }

// -------- EXCEPTIONS -------- //

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>
            ::InvalidOperation::InvalidOperation(const char * str) : std::runtime_error(str) {}

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::DeadlockException::DeadlockException() : InvalidOperation(
            "Invalid operation, possible deadlock: "
            "Cannot acquire lock after shared-lock in a single thread. "
            "Use upgradable-shared-lock instead, to lock afterwards."
        ) {}

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::PossibleLivelockException::PossibleLivelockException() : InvalidOperation(
            "Invalid operation, possible livelock: "
            "A thread is trying to lock after it has shared-locked. "
            "Use upgradable-shared-lock instead, to lock afterwards."
        ) {}

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>
            ::UpgradableSharedLockAfterSharedLockException::UpgradableSharedLockAfterSharedLockException() : InvalidOperation(
                "Invalid operation: "
                "Cannot acquire upgradable-shared-lock after shared-lock in a single thread."
        ) {}

// -------- ACTUAL MUTEX -------- //

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::RecursiveMutex() : HasOwner(false), LockGuardCount(0)
        {
            if constexpr (SupportsSharedLock)
                SharedOwnersRef = new Collections::Dictionary<std::thread::id, int, false>();
            if constexpr (SupportsUpgradableSharedLock)
            {
                HasUpgradableOwner = false;
                UpgradableLockGuardCount = 0;
            }
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::~RecursiveMutex()
        {
            if constexpr (SupportsSharedLock)
                delete SharedOwnersRef;
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        typename RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::LockGuard
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::GetLock()
        {
            return LockGuard(this);
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        bool RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::TryGetLock(LockGuard& GuardOut)
        {
            if (TryLock() != LockedByOtherThreads)
            {
                GuardOut = LockGuard(this);
                return true;
            }
            else return false;
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        typename RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::SharedLockGuard
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::_GetSharedLock()
        {
            if constexpr (SupportsSharedLock)
                return SharedLockGuard(this);
            else
                return SharedLockGuard(nullptr);
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        bool RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::_TryGetSharedLock(SharedLockGuard& GuardOut)
        {
            if constexpr (SupportsSharedLock)
            {
                if (TrySharedLock() != LockedByOtherThreads)
                {
                    GuardOut = SharedLockGuard(this);
                    return true;
                }
                else return false;
            }
            else
                return false;
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        typename RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::UpgradableSharedLockGuard
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::_GetUpgradableSharedLock()
        {
            if constexpr (SupportsUpgradableSharedLock)
                return UpgradableSharedLockGuard(this);
            else
                return UpgradableSharedLockGuard(nullptr);
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        bool RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::_TryGetUpgradableSharedLock(UpgradableSharedLockGuard& GuardOut)
        {
            if constexpr (SupportsUpgradableSharedLock)
            {
                if (TryUpgradableSharedLock() != LockedByOtherThreads)
                {
                    GuardOut = UpgradableSharedLockGuard(this);
                    return true;
                }
                else return false;
            }
            else
                return false;
        }

        // Lock - behind the scenes

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        void RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::LockByGuard()
        {
            std::unique_lock<std::mutex> m(StateMutex);
            LockOperation(m);
            LockGuardCount++;
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        inline bool RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::LockOperation(std::unique_lock<std::mutex>& m)
        {
            if (HasOwner && (Owner == std::this_thread::get_id()))
                return false;

            if constexpr (SupportsSharedLock)
                if (SharedOwnersRef->Contains(std::this_thread::get_id()))
                    throw DeadlockException();

            if constexpr (SupportsUpgradableSharedLock)
            {
                if (HasUpgradableOwner && (UpgradableOwner == std::this_thread::get_id()))
                    ConditionVariable.wait(m, [&] {
                        return !HasOwner && SharedOwnersRef->GetCount() == 0;
                    });
                else
                    ConditionVariable.wait(m, [&] {
                        return !HasOwner && SharedOwnersRef->GetCount() == 0 && !HasUpgradableOwner;
                    });
            }
            else
            {
                if constexpr (SupportsSharedLock)
                    ConditionVariable.wait(m, [&] {
                        return !HasOwner && SharedOwnersRef->GetCount() == 0;
                    });
                else
                    ConditionVariable.wait(m, [&] {
                        return !HasOwner;
                    });
            }

            // Replaces upgradable-shared-lock with lock if it exists only by this thread
            Owner = std::this_thread::get_id();
            HasOwner = true;
            return true;
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        typename RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::TryResult
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::TryLock()
        {
            std::lock_guard<std::mutex> guard(StateMutex);

            if (HasOwner)
                if (Owner == std::this_thread::get_id())
                    return LockedByThisThread;
                else
                    return LockedByOtherThreads;

            if constexpr (SupportsSharedLock)
            {
                if (SharedOwnersRef->Contains(std::this_thread::get_id()))
                    throw PossibleLivelockException();

                if (SharedOwnersRef->GetCount() > 0)
                    return LockedByOtherThreads;
            }

            if constexpr (SupportsUpgradableSharedLock)
                if (HasUpgradableOwner && (UpgradableOwner != std::this_thread::get_id()))
                    return LockedByOtherThreads;

            // Replaces upgradable-shared-lock with lock if it exists only by this thread
            Owner = std::this_thread::get_id();
            HasOwner = true;
            return LockSuccessful;
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        void RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::UnlockByGuard()
        {
            std::unique_lock<std::mutex> m(StateMutex);
            LockGuardCount--;
            if (LockGuardCount == 0)
                UnlockOperation(m); // May unlock m before returning
            else if (LockGuardCount < 0)
                throw std::logic_error(
                    "This is a bug if the LockGuardCount member is not modified. Current LockGuardCount value is: "
                    + std::to_string(LockGuardCount)
                    );
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        inline bool RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::UnlockOperation(std::unique_lock<std::mutex>& m)
        {
            if (HasOwner && (Owner == std::this_thread::get_id()))
            {
                // Replaces with upgradable-shared-lock if this->UpgradableSharedLock() has been called
                //                                      and this->UpgradableSharedUnlock() isn't called yet
                HasOwner = false;
                m.unlock();
                ConditionVariable.notify_all(); // worst case: multiple shared-locks waiting
                                                // all waiting cases: multiple shared-locks
                                                //                    single lock
                                                //                    single upgradable-lock
                return true;
            }
            return false;
        }

        // SharedLock - behind the scenes

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        void RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::SharedLockByGuard()
        {
            if constexpr (SupportsSharedLock)
            {
                std::unique_lock<std::mutex> m(StateMutex);
                SharedLockOperation(m);
                SharedOwnersRef->SetValue(std::this_thread::get_id(), SharedOwnersRef->GetValue(std::this_thread::get_id()) + 1);
            }
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        inline bool RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::SharedLockOperation(std::unique_lock<std::mutex>& m)
        {
            if constexpr (SupportsSharedLock)
            {
                if (SharedOwnersRef->Contains(std::this_thread::get_id()))
                    return false;

                if (HasOwner && (Owner == std::this_thread::get_id()))
                {
                    // The lock will be replaced by shared-lock on this->Unlock()
                    SharedOwnersRef->SetValue(std::this_thread::get_id(), 0);
                    return true;
                }

                ConditionVariable.wait(m, [&] { return !HasOwner; });
                SharedOwnersRef->SetValue(std::this_thread::get_id(), 0);
                return true;
            }
            else return false; // Dummy
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        typename RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::TryResult
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::TrySharedLock()
        {
            if constexpr (SupportsSharedLock)
            {
                std::lock_guard<std::mutex> guard(StateMutex);

                if (SharedOwnersRef->Contains(std::this_thread::get_id()))
                    return LockedByThisThread;
                if (HasOwner && (Owner != std::this_thread::get_id()))
                    return LockedByOtherThreads;

                SharedOwnersRef->SetValue(std::this_thread::get_id(), 0);
                return LockSuccessful;
            }
            else return LockedByOtherThreads; // Dummy
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        void RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::SharedUnlockByGuard()
        {
            if constexpr (SupportsSharedLock)
            {
                std::unique_lock<std::mutex> m(StateMutex);
                int SharedLockGuardsCount = SharedOwnersRef->GetValue(std::this_thread::get_id());
                SharedLockGuardsCount--;
                SharedOwnersRef->SetValue(std::this_thread::get_id(), SharedLockGuardsCount);
                if (SharedLockGuardsCount == 0)
                    SharedUnlockOperation(m); // May unlock m before returning
                else if (SharedLockGuardsCount < 0)
                    throw std::logic_error(
                        "This is a bug if the SharedOwners member is not modified. "
                        "Current SharedOwnersRef->GetValue(std::this_thread::get_id()) value is: "
                        + std::to_string(SharedLockGuardsCount)
                        );
            }
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        inline bool RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::SharedUnlockOperation(std::unique_lock<std::mutex>& m)
        {
            if constexpr (SupportsSharedLock)
            {
                if (SharedOwnersRef->Contains(std::this_thread::get_id()))
                {
                    SharedOwnersRef->Remove(std::this_thread::get_id());
                    m.unlock();
                    if constexpr (SupportsUpgradableSharedLock)
                        ConditionVariable.notify_all(); // worst case: single lock waiting after upgradable-shared-lock
                                                        //             while other locks wait (and cannot lock)
                                                        // other waiting cases: single lock
                    else
                        ConditionVariable.notify_one(); // worst case: single lock
                                                        // all waiting cases: single lock
                    return true;
                }
                return false;
            }
            else return false; // Dummy
        }

        // UpgradableSharedLock - behind the scenes
        // TODO: Review

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        void RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::UpgradableSharedLockByGuard()
        {
            if constexpr (SupportsUpgradableSharedLock)
            {
                std::unique_lock<std::mutex> m(StateMutex);
                UpgradableSharedLockOperation(m);
                UpgradableLockGuardCount++;
            }
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        inline bool RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::UpgradableSharedLockOperation(std::unique_lock<std::mutex>& m)
        {
            if constexpr (SupportsUpgradableSharedLock)
            {
                if (HasUpgradableOwner && (UpgradableOwner == std::this_thread::get_id()))
                    return false;

                if (HasOwner && (Owner == std::this_thread::get_id())) // And of course, && !HasUpgradableOwner
                {
                    // The lock will be replaced by upgradable-shared-lock on this->Unlock()
                    UpgradableOwner = std::this_thread::get_id();
                    HasUpgradableOwner = true;
                    return true;
                }

                if (SharedOwnersRef->Contains(std::this_thread::get_id()))
                    throw UpgradableSharedLockAfterSharedLockException();

                ConditionVariable.wait(m, [&] { return !HasUpgradableOwner && !HasOwner; });
                UpgradableOwner = std::this_thread::get_id();
                HasUpgradableOwner = true;
                return true;
            }
            else return false; // Dummy
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        typename RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::TryResult
        RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::TryUpgradableSharedLock()
        {
            if constexpr (SupportsUpgradableSharedLock)
            {
                std::lock_guard<std::mutex> guard(StateMutex);

                if (HasUpgradableOwner)
                    if (UpgradableOwner == std::this_thread::get_id())
                        return LockedByThisThread;
                    else
                        return LockedByOtherThreads;

                if (HasOwner && (Owner != std::this_thread::get_id()))
                    return LockedByOtherThreads;

                if (SharedOwnersRef->Contains(std::this_thread::get_id()))
                    throw UpgradableSharedLockAfterSharedLockException();

                UpgradableOwner = std::this_thread::get_id();
                HasUpgradableOwner = true;
                return LockSuccessful;
            }
            else return LockedByOtherThreads; // Dummy
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        void RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::UpgradableSharedUnlockByGuard()
        {
            if constexpr (SupportsUpgradableSharedLock)
            {
                std::unique_lock<std::mutex> m(StateMutex);
                UpgradableLockGuardCount--;
                if (UpgradableLockGuardCount == 0)
                    UpgradableSharedUnlockOperation(m); // May unlock m before returning
                else if (UpgradableLockGuardCount < 0)
                    throw std::logic_error(
                        "This is a bug if the UpgradableLockGuardCount member is not modified. Current UpgradableLockGuardCount value is: "
                        + std::to_string(UpgradableLockGuardCount)
                        );
            }
        }

        template <bool SupportsSharedLock, bool SupportsUpgradableSharedLock>
        inline bool RecursiveMutex<SupportsSharedLock, SupportsUpgradableSharedLock>::UpgradableSharedUnlockOperation(std::unique_lock<std::mutex>& m)
        {
            if constexpr (SupportsUpgradableSharedLock)
            {
                if (HasUpgradableOwner && (UpgradableOwner == std::this_thread::get_id()))
                {
                    HasUpgradableOwner = false;
                    m.unlock();
                    ConditionVariable.notify_one(); // worst case: single lock waiting
                                                    // all waiting case: single lock
                                                    //                   single upgradable-shared-lock
                    return true;
                }
                return false;
            }
            else return false; // Dummy
        }

        // Usable template parameters

        template class RecursiveMutex<false, false>;
        template class RecursiveMutex<true, false>;
        template class RecursiveMutex<true, true>;

        // Usable member functions

        template RecursiveMutex<true, false>::SharedLockGuard RecursiveMutex<true, false>::GetSharedLock();
        template RecursiveMutex<true, true>::SharedLockGuard  RecursiveMutex<true, true>::GetSharedLock();

        template bool RecursiveMutex<true, false>::TryGetSharedLock(RecursiveMutex<true, false>::SharedLockGuard&);
        template bool RecursiveMutex<true, true>::TryGetSharedLock(RecursiveMutex<true, true>::SharedLockGuard&);

        template RecursiveMutex<true, true>::UpgradableSharedLockGuard RecursiveMutex<true, true>::GetUpgradableSharedLock();

        template bool RecursiveMutex<true, true>::TryGetUpgradableSharedLock(RecursiveMutex<true, true>::UpgradableSharedLockGuard&);
    }
}
