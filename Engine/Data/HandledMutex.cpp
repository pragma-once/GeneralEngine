#include "../Engine.h"

namespace Engine
{
    namespace Data
    {
        HandledMutex::LockGuard::LockGuard() : m(nullptr) {}

        HandledMutex::LockGuard::LockGuard(LockGuard& op)
        {
            m = op.m;
            m->LockByGuard();
        }

        HandledMutex::LockGuard::LockGuard(LockGuard&& op)
        {
            m = std::exchange(op.m, nullptr);
        }

        HandledMutex::LockGuard& HandledMutex::LockGuard::operator=(LockGuard& op)
        {
            if (m != nullptr)
                m->UnlockByGuard();
            m = op.m;
            m->LockByGuard();
            return *this;
        }

        HandledMutex::LockGuard& HandledMutex::LockGuard::operator=(LockGuard&& op)
        {
            if (m != nullptr)
                m->UnlockByGuard();
            m = std::exchange(op.m, nullptr);
            return *this;
        }

        void HandledMutex::LockGuard::Unlock()
        {
            if (m != nullptr)
            {
                m->UnlockByGuard();
                m = nullptr;
            }
        }

        HandledMutex::LockGuard::~LockGuard()
        {
            if (m != nullptr)
                m->UnlockByGuard();
        }

        HandledMutex::LockGuard::LockGuard(HandledMutex * m) : m(m) { if (m != nullptr) m->LockByGuard(); }

        HandledMutex::SharedLockGuard::SharedLockGuard() : m(nullptr) {}

        HandledMutex::SharedLockGuard::SharedLockGuard(SharedLockGuard& op)
        {
            m = op.m;
            m->LockSharedByGuard();
        }

        HandledMutex::SharedLockGuard::SharedLockGuard(SharedLockGuard&& op)
        {
            m = std::exchange(op.m, nullptr);
        }

        HandledMutex::SharedLockGuard& HandledMutex::SharedLockGuard::operator=(SharedLockGuard& op)
        {
            if (m != nullptr)
                m->UnlockSharedByGuard();
            m = op.m;
            m->LockSharedByGuard();
            return *this;
        }

        HandledMutex::SharedLockGuard& HandledMutex::SharedLockGuard::operator=(SharedLockGuard&& op)
        {
            if (m != nullptr)
                m->UnlockSharedByGuard();
            m = std::exchange(op.m, nullptr);
            return *this;
        }

        void HandledMutex::SharedLockGuard::Unlock()
        {
            if (m != nullptr)
            {
                m->UnlockSharedByGuard();
                m = nullptr;
            }
        }

        HandledMutex::SharedLockGuard::~SharedLockGuard()
        {
            if (m != nullptr)
                m->UnlockSharedByGuard();
        }

        HandledMutex::SharedLockGuard::SharedLockGuard(HandledMutex * m) : m(m) { if (m != nullptr) m->LockSharedByGuard(); }

        HandledMutex::HandledMutex() : HasOwner(false), LockGuardCount(0)
        {
            SharedOwnersRef = new Collections::Dictionary<std::thread::id, int, false>();
        }

        HandledMutex::~HandledMutex()
        {
            delete SharedOwnersRef;
        }

        bool HandledMutex::Lock()
        {
            std::unique_lock<std::mutex> m(StateMutex);
            return LockOperation(m);
        }

        inline bool HandledMutex::LockOperation(std::unique_lock<std::mutex>& m)
        {
            if (HasOwner && (Owner == std::this_thread::get_id()))
                return false;

            bool IsSharedOwner = SharedOwnersRef->Contains(std::this_thread::get_id());

            ConditionVariable.wait(m, [&] {
                return !HasOwner && SharedOwnersRef->GetCount() <= (IsSharedOwner ? 1 : 0);
            });

            // Replaces shared lock with lock if it exists only by this thread
            Owner = std::this_thread::get_id();
            HasOwner = true;
            return true;
        }

        HandledMutex::TryResult HandledMutex::TryLock()
        {
            std::lock_guard<std::mutex> guard(StateMutex);

            if (HasOwner)
                if (Owner == std::this_thread::get_id())
                    return LockedByThisThread;
                else
                    return LockedByOtherThreads;

            bool IsSharedOwner = SharedOwnersRef->Contains(std::this_thread::get_id());
            if (SharedOwnersRef->GetCount() > (IsSharedOwner ? 1 : 0))
                return LockedByOtherThreads;

            Owner = std::this_thread::get_id();
            HasOwner = true;
            return LockSuccessful;
        }

        bool HandledMutex::Unlock()
        {
            std::unique_lock<std::mutex> m(StateMutex);
            return UnlockOperation(m); // May unlock m before returning
        }

        inline bool HandledMutex::UnlockOperation(std::unique_lock<std::mutex>& m)
        {
            if (HasOwner && (Owner == std::this_thread::get_id()))
            {
                // Replaces with shared lock if this->LockShared() was called
                //                           and this->UnlockShared() isn't called yet
                HasOwner = false;
                m.unlock();
                ConditionVariable.notify_all(); // worst case: multiple shared-locks waiting
                return true;
            }
            return false;
        }

        HandledMutex::LockGuard HandledMutex::GetLock()
        {
            return LockGuard(this);
        }

        bool HandledMutex::TryGetLock(LockGuard &GuardOut)
        {
            if (TryLock() != LockedByOtherThreads)
            {
                GuardOut = LockGuard(this);
                return true;
            }
            else return false;
        }

        bool HandledMutex::LockShared()
        {
            std::unique_lock<std::mutex> m(StateMutex);
            return LockSharedOperation(m);
        }

        inline bool HandledMutex::LockSharedOperation(std::unique_lock<std::mutex>& m)
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

        HandledMutex::TryResult HandledMutex::TryLockShared()
        {
            std::lock_guard<std::mutex> guard(StateMutex);

            if (SharedOwnersRef->Contains(std::this_thread::get_id()))
                return LockedByThisThread;
            if (HasOwner && (Owner != std::this_thread::get_id()))
                return LockedByOtherThreads;

            SharedOwnersRef->SetValue(std::this_thread::get_id(), 0);
            return LockSuccessful;
        }

        bool HandledMutex::UnlockShared()
        {
            std::unique_lock<std::mutex> m(StateMutex);
            return UnlockSharedOperation(m); // May unlock m before returning
        }

        inline bool HandledMutex::UnlockSharedOperation(std::unique_lock<std::mutex>& m)
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

        HandledMutex::SharedLockGuard HandledMutex::GetSharedLock()
        {
            return SharedLockGuard(this);
        }

        bool HandledMutex::TryGetSharedLock(SharedLockGuard &GuardOut)
        {
            if (TryLockShared() != LockedByOtherThreads)
            {
                GuardOut = SharedLockGuard(this);
                return true;
            }
            else return false;
        }

        void HandledMutex::LockByGuard()
        {
            std::unique_lock<std::mutex> m(StateMutex);
            LockOperation(m);
            LockGuardCount++;
        }

        void HandledMutex::UnlockByGuard()
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

        void HandledMutex::LockSharedByGuard()
        {
            std::unique_lock<std::mutex> m(StateMutex);
            LockSharedOperation(m);
            SharedOwnersRef->SetValue(std::this_thread::get_id(), SharedOwnersRef->GetValue(std::this_thread::get_id()) + 1);
        }

        void HandledMutex::UnlockSharedByGuard()
        {
            std::unique_lock<std::mutex> m(StateMutex);
            int SharedLockGuardsCount = SharedOwnersRef->GetValue(std::this_thread::get_id());
            SharedLockGuardsCount--;
            SharedOwnersRef->SetValue(std::this_thread::get_id(), SharedLockGuardsCount);
            if (SharedLockGuardsCount == 0)
                UnlockSharedOperation(m); // May unlock m before returning
            else if (SharedLockGuardsCount < 0)
                throw std::logic_error(
                    "This is a bug if the SharedOwners member is not modified. Current SharedOwnersRef->GetValue(std::this_thread::get_id()) value is: "
                    + std::to_string(SharedLockGuardsCount)
                    );
        }
    }
}
