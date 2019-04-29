#include "../Engine.h"

namespace Engine
{
    namespace Core
    {
        Module::Module() : Priority(0), isEnabled(true), loop(nullptr) {}

        Module::Module(int Priority) : Priority(Priority), isEnabled(true), loop(nullptr) {}

        Module::~Module() {};

        void Module::Enable()
        {
            if (!isEnabled)
            {
                isEnabled = true;
                if (loop != nullptr && loop.Get()->isRunning)
                    OnEnable();
            }
        }

        void Module::Disable()
        {
            if (isEnabled)
            {
                if (loop != nullptr && loop.Get()->isRunning)
                    OnDisable();
                isEnabled = false;
            }
        }

        bool Module::IsEnabled()
        {
            return isEnabled;
        }

        bool Module::IsRunning()
        {
            return loop != nullptr && isEnabled && loop.Get()->isRunning;
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
                throw std::logic_error("Cannot add one Module to multiple Loops.");
            this->loop = loop;
        }

        void Module::Release()
        {
            loop = nullptr;
        }

        void Module::_Start()
        {
            OnStart();
            if (isEnabled)
                OnEnable();
        }

        void Module::_Stop()
        {
            if (isEnabled)
                OnDisable();
            OnStop();
        }
    }
}
