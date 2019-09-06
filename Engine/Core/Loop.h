#pragma once

#include "../Engine.dec.h"
#include "../Data/Shared.h"
#include "../Data/Collections/List.h"
#include "../Data/Collections/PriorityQueue.h"

namespace Engine
{
    namespace Core
    {
        class Loop final
        {
            friend Module;
        public:
            /// @brief The modules that are going to be running.
            ///
            /// Add the modules to this list.
            Data::Collections::List<Module*> Modules;

            Loop();

            /// @brief Starts the loop.
            ///
            /// Note that it's not an async start.
            void Run();
            /// @brief Stops the loop.
            void Stop();

            /// @brief Checks if the loop is running.
            bool IsRunning();

            /// @brief Schedules to call a function.
            ///
            /// Will not call if the Loop is stopped before the call.
            /// Is executed right before 0-Priority Modules.
            ///
            /// @param Task The function that will be called.
            /// @param ExceptionHandler The function that will be called to handle exceptions thrown by Task.
            /// @param Time The time when the function will be called.
            ///        Time = 0 or Time <= CurrentTime results in calling the function shortly.
            void Schedule(
                std::function<void()> Task,
                std::function<void(std::exception&)> ExceptionHandler = nullptr,
                double Time = 0,
                ExecutionType ExecutionType = ExecutionType::BoundedAsync
            );
            /// @brief Schedules to call a function.
            ///
            /// Will not call if the Loop is stopped before the call.
            /// Is executed right before 0-Priority Modules.
            ///
            /// @param Task The function that will be called.
            /// @param ExceptionHandler The function that will be called to handle exceptions thrown by Task.
            /// @param Time The time when the function will be called.
            ///        Time = 0 or Time <= CurrentTime results in calling the function shortly.
            void Schedule(
                double Time,
                std::function<void()> Task,
                std::function<void(std::exception&)> ExceptionHandler = nullptr,
                ExecutionType ExecutionType = ExecutionType::BoundedAsync
            );
            /// @brief Schedules to call a function.
            ///
            /// Will not call if the Loop is stopped before the call.
            /// Is executed right before 0-Priority Modules.
            ///
            /// @param Task The function that will be called.
            /// @param ExceptionHandler The function that will be called to handle exceptions thrown by Task.
            /// @param Time The time when the function will be called.
            ///        Time = 0 or Time <= CurrentTime results in calling the function shortly.
            void Schedule(
                double Time,
                ExecutionType ExecutionType,
                std::function<void()> Task,
                std::function<void(std::exception&)> ExceptionHandler = nullptr
            );
        private:
            int ZeroPriorityModulesStartIndex;
            int ZeroPriorityModulesEndIndex;

            Data::Shared<bool, true> isRunning;
            Data::Shared<double> Time;
            Data::Shared<double> TimeDiff;
            Data::Shared<float> TimeFloat;
            Data::Shared<float> TimeDiffFloat;
            Data::Shared<bool> ShouldStop;

            struct ScheduledJob
            {
                std::function<void()> Task;
                std::function<void(std::exception&)> ExceptionHandler;
                ScheduledJob() {}
                ScheduledJob(
                    std::function<void()> Task,
                    std::function<void(std::exception&)> ExceptionHandler
                );
            };

            Data::Collections::PriorityQueue<std::pair<ExecutionType, ScheduledJob>, double> Schedules;

            enum ModulesEditType : std::int_fast8_t { Add, Replace, Remove, Clear };
            Data::Collections::Queue<std::tuple<ModulesEditType, int, Module*>> ToEditModules;
            Data::Collections::List<Module*> UpdatingModules;
            Data::Collections::Queue<std::tuple<ExecutionType, ScheduledJob, double>> ToSchedule;

            void ExecuteScheduledJob(ScheduledJob&);
            void ExecuteUpdate(Module*);
        };
    }
}
