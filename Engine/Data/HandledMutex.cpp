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
            std::unique_lock<std::mutex> m(MembersMutex);
            return LockOperation(m);
        }

        inline bool HandledMutex::LockOperation(std::unique_lock<std::mutex>& m)
        {
            if (HasOwner && (Owner == std::this_thread::get_id()))
                return false;

            bool IsSharedOwner = SharedOwnersRef->Contains(std::this_thread::get_id());

            while (HasOwner || SharedOwnersRef->GetCount() > (IsSharedOwner ? 1 : 0))
            {
                m.unlock();
                std::this_thread::yield();
                m.lock();
            }
            if (IsSharedOwner)
                // Replace shared lock with lock if it exists only by this thread
                Mutex.unlock_shared();
            Mutex.lock();
            Owner = std::this_thread::get_id();
            HasOwner = true;
            return true;
        }

        bool HandledMutex::TryLock()
        {
            std::lock_guard<std::mutex> guard(MembersMutex);
            if (Mutex.try_lock())
            {
                Owner = std::this_thread::get_id();
                HasOwner = true;
                return true;
            }
            return false;
        }

        bool HandledMutex::Unlock()
        {
            std::lock_guard<std::mutex> guard(MembersMutex);
            return UnlockOperation();
        }

        inline bool HandledMutex::UnlockOperation()
        {
            if (HasOwner && (Owner == std::this_thread::get_id()))
            {
                Mutex.unlock();
                if (SharedOwnersRef->Contains(std::this_thread::get_id()))
                    // Replace with shared lock if this->LockShared() was called
                    //                          and this->UnlockShared() isn't called yet
                    Mutex.lock_shared();
                HasOwner = false;
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
            if (TryLock())
            {
                GuardOut = LockGuard(this);
                return true;
            }
            else return false;
        }

        bool HandledMutex::LockShared()
        {
            std::unique_lock<std::mutex> m(MembersMutex);

            return LockSharedOperation(m);
        }

        inline bool HandledMutex::LockSharedOperation(std::unique_lock<std::mutex>& m)
        {
            if (SharedOwnersRef->Contains(std::this_thread::get_id()))
                return false;

            if (HasOwner && (Owner == std::this_thread::get_id()))
            {
                // Mutex.lock_shared() will be called on this->Unlock()
                SharedOwnersRef->SetValue(std::this_thread::get_id(), 0);
                return true;
            }

            while (HasOwner)
            {
                m.unlock();
                std::this_thread::yield();
                m.lock();
            }
            Mutex.lock_shared();
            SharedOwnersRef->SetValue(std::this_thread::get_id(), 0);
            return true;
        }

        bool HandledMutex::TryLockShared()
        {
            std::lock_guard<std::mutex> guard(MembersMutex);

            if (SharedOwnersRef->Contains(std::this_thread::get_id()))
                return false;

            if (Mutex.try_lock_shared())
            {
                SharedOwnersRef->SetValue(std::this_thread::get_id(), 0);
                return true;
            }
            return false;
        }

        bool HandledMutex::UnlockShared()
        {
            std::lock_guard<std::mutex> guard(MembersMutex);
            return UnlockSharedOperation();
        }

        inline bool HandledMutex::UnlockSharedOperation()
        {
            if (SharedOwnersRef->Contains(std::this_thread::get_id()))
            {
                if (!(HasOwner && (Owner == std::this_thread::get_id())))
                    Mutex.unlock_shared();
                // else, Mutex.unlock_shared() was called by this->Lock() already
                SharedOwnersRef->Remove(std::this_thread::get_id());
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
            if (TryLockShared())
            {
                GuardOut = SharedLockGuard(this);
                return true;
            }
            else return false;
        }

        void HandledMutex::LockByGuard()
        {
            std::unique_lock<std::mutex> m(MembersMutex);
            LockOperation(m);
            LockGuardCount++;
        }

        void HandledMutex::UnlockByGuard()
        {
            std::lock_guard<std::mutex> guard(MembersMutex);
            LockGuardCount--;
            if (LockGuardCount == 0)
                UnlockOperation();
            else if (LockGuardCount < 0)
                throw std::logic_error(
                    "This is a bug if the LockGuardCount member is not modified. Current LockGuardCount value is: "
                    + std::to_string(LockGuardCount)
                    );
        }

        void HandledMutex::LockSharedByGuard()
        {
            std::unique_lock<std::mutex> m(MembersMutex);
            LockSharedOperation(m);
            SharedOwnersRef->SetValue(std::this_thread::get_id(), SharedOwnersRef->GetValue(std::this_thread::get_id()) + 1);
        }

        void HandledMutex::UnlockSharedByGuard()
        {
            std::lock_guard<std::mutex> guard(MembersMutex);
            int SharedLockGuardCounts = SharedOwnersRef->GetValue(std::this_thread::get_id());
            SharedLockGuardCounts--;
            SharedOwnersRef->SetValue(std::this_thread::get_id(), SharedLockGuardCounts);
            if (SharedLockGuardCounts == 0)
                UnlockSharedOperation();
            else if (SharedLockGuardCounts < 0)
                throw std::logic_error(
                    "This is a bug if the SharedOwners member is not modified. Current SharedOwnersRef->GetValue(std::this_thread::get_id()) value is: "
                    + std::to_string(SharedLockGuardCounts)
                    );
        }
    }
}
