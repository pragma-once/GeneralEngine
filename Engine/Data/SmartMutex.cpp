#include "../Engine.h"

namespace Engine
{
    namespace Data
    {

// -------- LOCK GUARD -------- //

        SmartMutex::LockGuard::LockGuard() : m(nullptr) {}

        SmartMutex::LockGuard::LockGuard(LockGuard& op)
        {
            m = op.m;
            m->LockByGuard();
        }

        SmartMutex::LockGuard::LockGuard(LockGuard&& op)
        {
            m = std::exchange(op.m, nullptr);
        }

        SmartMutex::LockGuard& SmartMutex::LockGuard::operator=(LockGuard& op)
        {
            if (m != nullptr)
                m->UnlockByGuard();
            m = op.m;
            m->LockByGuard();
            return *this;
        }

        SmartMutex::LockGuard& SmartMutex::LockGuard::operator=(LockGuard&& op)
        {
            if (m != nullptr)
                m->UnlockByGuard();
            m = std::exchange(op.m, nullptr);
            return *this;
        }

        void SmartMutex::LockGuard::Unlock()
        {
            if (m != nullptr)
            {
                m->UnlockByGuard();
                m = nullptr;
            }
        }

        SmartMutex::LockGuard::~LockGuard()
        {
            if (m != nullptr)
                m->UnlockByGuard();
        }

        SmartMutex::LockGuard::LockGuard(SmartMutex * m) : m(m) { if (m != nullptr) m->LockByGuard(); }

// -------- SHARED-LOCK GUARD -------- //

        SmartMutex::SharedLockGuard::SharedLockGuard() : m(nullptr) {}

        SmartMutex::SharedLockGuard::SharedLockGuard(SharedLockGuard& op)
        {
            m = op.m;
            m->SharedLockByGuard();
        }

        SmartMutex::SharedLockGuard::SharedLockGuard(SharedLockGuard&& op)
        {
            m = std::exchange(op.m, nullptr);
        }

        SmartMutex::SharedLockGuard& SmartMutex::SharedLockGuard::operator=(SharedLockGuard& op)
        {
            if (m != nullptr)
                m->SharedUnlockByGuard();
            m = op.m;
            m->SharedLockByGuard();
            return *this;
        }

        SmartMutex::SharedLockGuard& SmartMutex::SharedLockGuard::operator=(SharedLockGuard&& op)
        {
            if (m != nullptr)
                m->SharedUnlockByGuard();
            m = std::exchange(op.m, nullptr);
            return *this;
        }

        void SmartMutex::SharedLockGuard::Unlock()
        {
            if (m != nullptr)
            {
                m->SharedUnlockByGuard();
                m = nullptr;
            }
        }

        SmartMutex::SharedLockGuard::~SharedLockGuard()
        {
            if (m != nullptr)
                m->SharedUnlockByGuard();
        }

        SmartMutex::SharedLockGuard::SharedLockGuard(SmartMutex * m) : m(m) { if (m != nullptr) m->SharedLockByGuard(); }

// -------- DEADLOCK EXCEPTION -------- //

        SmartMutex::DeadlockException::DeadlockException() : std::runtime_error(
            "Deadlock occurred: "
            "2 or more threads are trying to lock after they have shared-locked."
        ) {}

// -------- ACTUAL MUTEX -------- //

        SmartMutex::SmartMutex() : HasOwner(false), LockGuardCount(0), RequestingToLockWhileSharedLocked(false)
        {
            SharedOwnersRef = new Collections::Dictionary<std::thread::id, int, false>();
        }

        SmartMutex::~SmartMutex()
        {
            delete SharedOwnersRef;
        }

        bool SmartMutex::Lock()
        {
            std::unique_lock<std::mutex> m(StateMutex);
            return LockOperation(m);
        }

        inline bool SmartMutex::LockOperation(std::unique_lock<std::mutex>& m)
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

        SmartMutex::TryResult SmartMutex::TryLock()
        {
            std::lock_guard<std::mutex> guard(StateMutex);

            if (HasOwner)
                if (Owner == std::this_thread::get_id())
                    return LockedByThisThread;
                else
                    return LockedByOtherThreads;

            bool IsSharedOwner = SharedOwnersRef->Contains(std::this_thread::get_id());

            if (IsSharedOwner && RequestingToLockWhileSharedLocked)
                throw DeadlockException();

            if (SharedOwnersRef->GetCount() > (IsSharedOwner ? 1 : 0))
                return LockedByOtherThreads;

            Owner = std::this_thread::get_id();
            HasOwner = true;
            return LockSuccessful;
        }

        bool SmartMutex::Unlock()
        {
            std::unique_lock<std::mutex> m(StateMutex);
            return UnlockOperation(m); // May unlock m before returning
        }

        inline bool SmartMutex::UnlockOperation(std::unique_lock<std::mutex>& m)
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

        SmartMutex::LockGuard SmartMutex::GetLock()
        {
            return LockGuard(this);
        }

        bool SmartMutex::TryGetLock(LockGuard &GuardOut)
        {
            if (TryLock() != LockedByOtherThreads)
            {
                GuardOut = LockGuard(this);
                return true;
            }
            else return false;
        }

        bool SmartMutex::SharedLock()
        {
            std::unique_lock<std::mutex> m(StateMutex);
            return SharedLockOperation(m);
        }

        inline bool SmartMutex::SharedLockOperation(std::unique_lock<std::mutex>& m)
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

        SmartMutex::TryResult SmartMutex::TrySharedLock()
        {
            std::lock_guard<std::mutex> guard(StateMutex);

            if (SharedOwnersRef->Contains(std::this_thread::get_id()))
                return LockedByThisThread;
            if (HasOwner && (Owner != std::this_thread::get_id()))
                return LockedByOtherThreads;

            SharedOwnersRef->SetValue(std::this_thread::get_id(), 0);
            return LockSuccessful;
        }

        bool SmartMutex::SharedUnlock()
        {
            std::unique_lock<std::mutex> m(StateMutex);
            return SharedUnlockOperation(m); // May unlock m before returning
        }

        inline bool SmartMutex::SharedUnlockOperation(std::unique_lock<std::mutex>& m)
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

        SmartMutex::SharedLockGuard SmartMutex::GetSharedLock()
        {
            return SharedLockGuard(this);
        }

        bool SmartMutex::TryGetSharedLock(SharedLockGuard &GuardOut)
        {
            if (TrySharedLock() != LockedByOtherThreads)
            {
                GuardOut = SharedLockGuard(this);
                return true;
            }
            else return false;
        }

        void SmartMutex::LockByGuard()
        {
            std::unique_lock<std::mutex> m(StateMutex);
            LockOperation(m);
            LockGuardCount++;
        }

        void SmartMutex::UnlockByGuard()
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

        void SmartMutex::SharedLockByGuard()
        {
            std::unique_lock<std::mutex> m(StateMutex);
            SharedLockOperation(m);
            SharedOwnersRef->SetValue(std::this_thread::get_id(), SharedOwnersRef->GetValue(std::this_thread::get_id()) + 1);
        }

        void SmartMutex::SharedUnlockByGuard()
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
}
