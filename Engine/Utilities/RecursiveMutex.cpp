#include "../Engine.h"

namespace Engine
{
    namespace Utilities
    {

// -------- LOCK GUARD -------- //

        RecursiveMutex::LockGuard::LockGuard() : m(nullptr) {}

        RecursiveMutex::LockGuard::LockGuard(const LockGuard& op)
        {
            m = op.m;
            m->LockByGuard();
        }

        RecursiveMutex::LockGuard::LockGuard(LockGuard&& op)
        {
            m = std::exchange(op.m, nullptr);
        }

        RecursiveMutex::LockGuard& RecursiveMutex::LockGuard::operator=(const LockGuard& op)
        {
            if (m != nullptr)
                m->UnlockByGuard();
            m = op.m;
            m->LockByGuard();
            return *this;
        }

        RecursiveMutex::LockGuard& RecursiveMutex::LockGuard::operator=(LockGuard&& op)
        {
            if (m != nullptr)
                m->UnlockByGuard();
            m = std::exchange(op.m, nullptr);
            return *this;
        }

        void RecursiveMutex::LockGuard::Unlock()
        {
            if (m != nullptr)
            {
                m->UnlockByGuard();
                m = nullptr;
            }
        }

        RecursiveMutex::LockGuard::~LockGuard()
        {
            if (m != nullptr)
                m->UnlockByGuard();
        }

        RecursiveMutex::LockGuard::LockGuard(RecursiveMutex * m) : m(m) { if (m != nullptr) m->LockByGuard(); }

// -------- SHARED-LOCK GUARD -------- //

        RecursiveMutex::SharedLockGuard::SharedLockGuard() : m(nullptr) {}

        RecursiveMutex::SharedLockGuard::SharedLockGuard(const SharedLockGuard& op)
        {
            m = op.m;
            m->SharedLockByGuard();
        }

        RecursiveMutex::SharedLockGuard::SharedLockGuard(SharedLockGuard&& op)
        {
            m = std::exchange(op.m, nullptr);
        }

        RecursiveMutex::SharedLockGuard& RecursiveMutex::SharedLockGuard::operator=(const SharedLockGuard& op)
        {
            if (m != nullptr)
                m->SharedUnlockByGuard();
            m = op.m;
            m->SharedLockByGuard();
            return *this;
        }

        RecursiveMutex::SharedLockGuard& RecursiveMutex::SharedLockGuard::operator=(SharedLockGuard&& op)
        {
            if (m != nullptr)
                m->SharedUnlockByGuard();
            m = std::exchange(op.m, nullptr);
            return *this;
        }

        void RecursiveMutex::SharedLockGuard::Unlock()
        {
            if (m != nullptr)
            {
                m->SharedUnlockByGuard();
                m = nullptr;
            }
        }

        RecursiveMutex::SharedLockGuard::~SharedLockGuard()
        {
            if (m != nullptr)
                m->SharedUnlockByGuard();
        }

        RecursiveMutex::SharedLockGuard::SharedLockGuard(RecursiveMutex * m) : m(m) { if (m != nullptr) m->SharedLockByGuard(); }

// -------- EXCEPTIONS -------- //

        RecursiveMutex::DeadlockException::DeadlockException() : std::runtime_error(
            "Deadlock occurred: "
            "2 or more threads are locking after they have shared-locked."
        ) {}

        RecursiveMutex::PossibleLivelockException::PossibleLivelockException() : std::runtime_error(
            "Livelock may have occurred: "
            "2 or more threads are trying to lock after they have shared-locked."
        ) {}

// -------- ACTUAL MUTEX -------- //

        RecursiveMutex::RecursiveMutex() : HasOwner(false), LockGuardCount(0), RequestingToLockWhileSharedLocked(false)
        {
            SharedOwnersRef = new Collections::Dictionary<std::thread::id, int, false>();
        }

        RecursiveMutex::~RecursiveMutex()
        {
            delete SharedOwnersRef;
        }

        RecursiveMutex::LockGuard RecursiveMutex::GetLock()
        {
            return LockGuard(this);
        }

        bool RecursiveMutex::TryGetLock(LockGuard &GuardOut)
        {
            if (TryLock() != LockedByOtherThreads)
            {
                GuardOut = LockGuard(this);
                return true;
            }
            else return false;
        }

        RecursiveMutex::SharedLockGuard RecursiveMutex::GetSharedLock()
        {
            return SharedLockGuard(this);
        }

        bool RecursiveMutex::TryGetSharedLock(SharedLockGuard &GuardOut)
        {
            if (TrySharedLock() != LockedByOtherThreads)
            {
                GuardOut = SharedLockGuard(this);
                return true;
            }
            else return false;
        }

        void RecursiveMutex::LockByGuard()
        {
            std::unique_lock<std::mutex> m(StateMutex);
            LockOperation(m);
            LockGuardCount++;
        }

        inline bool RecursiveMutex::LockOperation(std::unique_lock<std::mutex>& m)
        {
            if (HasOwner && (Owner == std::this_thread::get_id()))
                return false;

            bool IsSharedOwner = SharedOwnersRef->Contains(std::this_thread::get_id());

            if (IsSharedOwner)
            {
                if (RequestingToLockWhileSharedLocked)
                    throw DeadlockException();
                RequestingToLockWhileSharedLocked = true;
            }

            ConditionVariable.wait(m, [&] {
                return !HasOwner && SharedOwnersRef->GetCount() <= (IsSharedOwner ? 1 : 0);
            });

            if (IsSharedOwner)
                RequestingToLockWhileSharedLocked = false;

            // Replaces shared lock with lock if it exists only by this thread
            Owner = std::this_thread::get_id();
            HasOwner = true;
            return true;
        }

        RecursiveMutex::TryResult RecursiveMutex::TryLock()
        {
            std::lock_guard<std::mutex> guard(StateMutex);

            if (HasOwner)
                if (Owner == std::this_thread::get_id())
                    return LockedByThisThread;
                else
                    return LockedByOtherThreads;

            bool IsSharedOwner = SharedOwnersRef->Contains(std::this_thread::get_id());

            if (IsSharedOwner && RequestingToLockWhileSharedLocked)
                throw PossibleLivelockException();

            if (SharedOwnersRef->GetCount() > (IsSharedOwner ? 1 : 0))
                return LockedByOtherThreads;

            Owner = std::this_thread::get_id();
            HasOwner = true;
            return LockSuccessful;
        }

        void RecursiveMutex::UnlockByGuard()
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

        inline bool RecursiveMutex::UnlockOperation(std::unique_lock<std::mutex>& m)
        {
            if (HasOwner && (Owner == std::this_thread::get_id()))
            {
                // Replaces with shared lock if this->SharedLock() was called
                //                           and this->SharedUnlock() isn't called yet
                HasOwner = false;
                m.unlock();
                ConditionVariable.notify_all(); // worst case: multiple shared-locks waiting
                return true;
            }
            return false;
        }

        void RecursiveMutex::SharedLockByGuard()
        {
            std::unique_lock<std::mutex> m(StateMutex);
            SharedLockOperation(m);
            SharedOwnersRef->SetValue(std::this_thread::get_id(), SharedOwnersRef->GetValue(std::this_thread::get_id()) + 1);
        }

        inline bool RecursiveMutex::SharedLockOperation(std::unique_lock<std::mutex>& m)
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

        RecursiveMutex::TryResult RecursiveMutex::TrySharedLock()
        {
            std::lock_guard<std::mutex> guard(StateMutex);

            if (SharedOwnersRef->Contains(std::this_thread::get_id()))
                return LockedByThisThread;
            if (HasOwner && (Owner != std::this_thread::get_id()))
                return LockedByOtherThreads;

            SharedOwnersRef->SetValue(std::this_thread::get_id(), 0);
            return LockSuccessful;
        }

        void RecursiveMutex::SharedUnlockByGuard()
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

        inline bool RecursiveMutex::SharedUnlockOperation(std::unique_lock<std::mutex>& m)
        {
            if (SharedOwnersRef->Contains(std::this_thread::get_id()))
            {
                SharedOwnersRef->Remove(std::this_thread::get_id());
                m.unlock();
                ConditionVariable.notify_one(); // worst-case: one lock waiting
                return true;
            }
            return false;
        }
    }
}
