#include "../Engine.h"

namespace Engine
{
    namespace Core
    {
        Module::Module() : Priority(0), isActive(true), loop(nullptr) {}

        Module::Module(int Priority) : Priority(Priority), isActive(true), loop(nullptr) {}

        Module::~Module() {};

        void Module::Activate()
        {
            if (!isActive)
            {
                isActive = true;
                if (loop != nullptr && loop.Get()->isRunning)
                    OnActivate();
            }
        }

        void Module::Deactivate()
        {
            if (isActive)
            {
                if (loop != nullptr && loop.Get()->isRunning)
                    OnDeactivate();
                isActive = false;
            }
        }

        bool Module::IsActive()
        {
            return isActive;
        }

        bool Module::IsRunning()
        {
            return loop != nullptr && isActive && loop.Get()->isRunning;
        }

        int Module::GetPriority()
        {
            return Priority;
        }

        double Module::GetTime()
        {
            if (loop == nullptr)
                return 0;
            return loop.Get()->Time;
        }

        double Module::GetTimeDiff()
        {
            if (loop == nullptr)
                return 0;
            return loop.Get()->TimeDiff;
        }

        float Module::GetTimeFloat()
        {
            if (loop == nullptr)
                return 0;
            return loop.Get()->TimeFloat;
        }

        float Module::GetTimeDiffFloat()
        {
            if (loop == nullptr)
                return 0;
            return loop.Get()->TimeDiffFloat;
        }

        Loop * Module::GetLoop()
        {
            return loop;
        }

        void Module::Acquire(Loop * loop)
        {
            if (this->loop != nullptr)
                throw std::logic_error("Cannot add one Module to multiple loop[s].");
            this->loop = loop;
        }

        void Module::Release()
        {
            loop = nullptr;
        }

        void Module::_Start()
        {
            OnStart();
            if (isActive)
                OnActivate();
        }

        void Module::_Stop()
        {
            if (isActive)
                OnDeactivate();
            OnStop();
        }
    }
}
