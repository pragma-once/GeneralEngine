#include "../Engine.h"

namespace Engine
{
    namespace Core
    {
        Module::Module(std::int_fast8_t Priority) : Priority(Priority >= -128 ?
                                                        (Priority <= 127 ? Priority : 127)
                                                        : -128),
                                                    isEnabled(true), loop(nullptr) {}

        Module::~Module() {}

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

        ExecutionType Module::GetExecutionType()
        {
            return ExecutionType::BoundedAsync;
        }

        void Module::OnException(std::exception& e) {} // ignore

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

        float Module::GetTimeAsFloat()
        {
            if (loop == nullptr)
                return 0;
            return loop.Get()->TimeAsFloat;
        }

        float Module::GetTimeDiffAsFloat()
        {
            if (loop == nullptr)
                return 0;
            return loop.Get()->TimeDiffAsFloat;
        }

        double Module::GetPresentTime()
        {
            if (loop == nullptr)
                return 0;
            if (!loop.Get()->isRunning)
                return 0;
            auto duration = std::chrono::steady_clock::now() - loop.Get()->StartTime.Get();
            return (double)std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000000.0;
        }

        Loop * Module::GetLoop()
        {
            return loop;
        }

        void Module::Schedule(
                std::function<void()> Task,
                double Time,
                ExecutionType ExecutionType
        ) {
            if (GetLoop() == nullptr)
                throw std::runtime_error("No loop to schedule in.");
            GetLoop()->Schedule(Time, ExecutionType, Task, [&](std::exception& e) { OnException(e); });
        }

        void Module::Schedule(
                double Time,
                std::function<void()> Task,
                ExecutionType ExecutionType
        ) {
            if (GetLoop() == nullptr)
                throw std::runtime_error("No loop to schedule in.");
            GetLoop()->Schedule(Time, ExecutionType, Task, [&](std::exception& e) { OnException(e); });
        }

        void Module::Schedule(
                double Time,
                ExecutionType ExecutionType,
                std::function<void()> Task
        ) {
            if (GetLoop() == nullptr)
                throw std::runtime_error("No loop to schedule in.");
            GetLoop()->Schedule(Time, ExecutionType, Task, [&](std::exception& e) { OnException(e); });
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
