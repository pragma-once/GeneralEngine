#include "../Engine.h"

namespace Engine
{
    namespace Core
    {
        Behavior::Behavior() : Priority(0), isActive(true), container(nullptr) {}

        Behavior::Behavior(int Priority) : Priority(Priority), isActive(true), container(nullptr) {}

        void Behavior::Activate()
        {
            if (!isActive)
            {
                isActive = true;
                if (container != nullptr && container->isRunning)
                    OnActivate();
            }
        }

        void Behavior::Deactivate()
        {
            if (isActive)
            {
                if (container != nullptr && container->isRunning)
                    OnDeactivate();
                isActive = false;
            }
        }

        bool Behavior::IsActive()
        {
            return isActive;
        }

        int Behavior::GetPriority()
        {
            return Priority;
        }

        double Behavior::GetTime()
        {
            if (container == nullptr)
                return 0;
            return container->Time;
        }

        double Behavior::GetTimeDiff()
        {
            if (container == nullptr)
                return 0;
            return container->TimeDiff;
        }

        float Behavior::GetTimeFloat()
        {
            if (container == nullptr)
                return 0;
            return container->TimeFloat;
        }

        float Behavior::GetTimeDiffFloat()
        {
            if (container == nullptr)
                return 0;
            return container->TimeDiffFloat;
        }

        Container * Behavior::GetContainer()
        {
            return container;
        }

        void Behavior::Acquire(Container * container)
        {
            if (this->container != nullptr)
                throw std::logic_error("Cannot add one Behavior to multiple Container[s].");
            this->container = container;
        }

        void Behavior::Release()
        {
            container = nullptr;
        }

        void Behavior::_Start()
        {
            Start();
            if (isActive)
                OnActivate();
        }

        void Behavior::_End()
        {
            if (isActive)
                OnDeactivate();
            End();
        }
    }
}