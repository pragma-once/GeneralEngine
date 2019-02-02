#include "../Engine.h"

namespace Engine
{
    namespace Core
    {
        Module::Module() : Priority(0), isActive(true), container(nullptr) {}

        Module::Module(int Priority) : Priority(Priority), isActive(true), container(nullptr) {}

        void Module::Activate()
        {
            if (!isActive)
            {
                isActive = true;
                if (container != nullptr && container.Get()->isRunning)
                    OnActivate();
            }
        }

        void Module::Deactivate()
        {
            if (isActive)
            {
                if (container != nullptr && container.Get()->isRunning)
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
            return container != nullptr && isActive && container.Get()->isRunning;
        }

        int Module::GetPriority()
        {
            return Priority;
        }

        double Module::GetTime()
        {
            if (container == nullptr)
                return 0;
            return container.Get()->Time;
        }

        double Module::GetTimeDiff()
        {
            if (container == nullptr)
                return 0;
            return container.Get()->TimeDiff;
        }

        float Module::GetTimeFloat()
        {
            if (container == nullptr)
                return 0;
            return container.Get()->TimeFloat;
        }

        float Module::GetTimeDiffFloat()
        {
            if (container == nullptr)
                return 0;
            return container.Get()->TimeDiffFloat;
        }

        Container * Module::GetContainer()
        {
            return container;
        }

        void Module::Acquire(Container * container)
        {
            if (this->container != nullptr)
                throw std::logic_error("Cannot add one Module to multiple Container[s].");
            this->container = container;
        }

        void Module::Release()
        {
            container = nullptr;
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
