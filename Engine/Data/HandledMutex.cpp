#include "../Engine.h"

namespace Engine
{
    namespace Data
    {
        HandledMutex::LockGuard& HandledMutex::LockGuard::operator=(const LockGuard& op)
        {
            if (m != nullptr)
                m->Unlock();
            m = op.m;
            return *this;
        }

        HandledMutex::LockGuard::LockGuard() : m(nullptr) {}

        HandledMutex::LockGuard::~LockGuard()
        {
            if (m != nullptr)
                m->Unlock();
        }

        HandledMutex::LockGuard::LockGuard(HandledMutex * m) : m(m) {}

        HandledMutex::SharedLockGuard& HandledMutex::SharedLockGuard::operator=(const SharedLockGuard& op)
        {
            if (m != nullptr)
                m->UnlockShared();
            m = op.m;
            return *this;
        }

        HandledMutex::SharedLockGuard::SharedLockGuard() : m(nullptr) {}

        HandledMutex::SharedLockGuard::~SharedLockGuard()
        {
            if (m != nullptr)
                m->UnlockShared();
        }

        HandledMutex::SharedLockGuard::SharedLockGuard(HandledMutex * m) : m(m) {}

        HandledMutex::HandledMutex()
        {
            HasOwner = false;
            SharedOwners = new Collections::List<std::thread::id, false>();
        }

        bool HandledMutex::Lock()
        {
            std::unique_lock<std::mutex> m(OwnerMutex);

            if (HasOwner && (Owner == std::this_thread::get_id()))
                return false;

            bool IsSharedOwner = SharedOwners->Exists(std::this_thread::get_id());

            while (HasOwner || SharedOwners->GetCount() > (IsSharedOwner ? 1 : 0))
            {
                m.unlock();
                std::this_thread::yield();
                m.lock();
            }
            if (IsSharedOwner)
                Mutex.unlock_shared();
            Mutex.lock();
            Owner = std::this_thread::get_id();
            HasOwner = true;
            return true;
        }

        bool HandledMutex::TryLock()
        {
            std::lock_guard<std::mutex> guard(OwnerMutex);
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
            std::lock_guard<std::mutex> guard(OwnerMutex);
            if (HasOwner && (Owner == std::this_thread::get_id()))
            {
                Mutex.unlock();
                if (SharedOwners->Exists(std::this_thread::get_id()))
                    Mutex.lock_shared();
                HasOwner = false;
                return true;
            }
            return false;
        }

        HandledMutex::LockGuard HandledMutex::GetLock()
        {
            if (Lock())
                return LockGuard(this);
            else
                return LockGuard(nullptr);
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
            std::unique_lock<std::mutex> m(OwnerMutex);

            if (SharedOwners->Exists(std::this_thread::get_id()) || (HasOwner && (Owner == std::this_thread::get_id())))
                return false;

            while (HasOwner)
            {
                m.unlock();
                std::this_thread::yield();
                m.lock();
            }
            Mutex.lock_shared();
            SharedOwners->Add(std::this_thread::get_id());
            return true;
        }

        bool HandledMutex::TryLockShared()
        {
            std::lock_guard<std::mutex> guard(OwnerMutex);

            if (SharedOwners->Exists(std::this_thread::get_id()))
                return false;

            if (Mutex.try_lock_shared())
            {
                SharedOwners->Add(std::this_thread::get_id());
                return true;
            }
            return false;
        }

        bool HandledMutex::UnlockShared()
        {
            std::lock_guard<std::mutex> guard(OwnerMutex);
            if (SharedOwners->Exists(std::this_thread::get_id()))
            {
                if (!(HasOwner && (Owner == std::this_thread::get_id())))
                    Mutex.unlock_shared();
                SharedOwners->Remove(std::this_thread::get_id());
                return true;
            }
            return false;
        }

        HandledMutex::SharedLockGuard HandledMutex::GetSharedLock()
        {
            if (LockShared())
                return SharedLockGuard(this);
            else
                return SharedLockGuard(nullptr);
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
    }
}