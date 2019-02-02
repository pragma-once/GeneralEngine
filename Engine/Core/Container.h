#pragma once

#include "../Engine.dec.h"
#include "../Data/Shared.h"
#include "../Data/Collections/List.h"
#include "../Data/Collections/PriorityQueue.h"

namespace Engine
{
    namespace Core
    {
        class Container // TODO: Rename
        {
            friend Module;
        public:
            /// @brief The modules that are going to be running.
            ///
            /// Add the modules to this list.
            Data::Collections::List<Module*> Modules;

            Container();

            /// @brief Starts the loop.
            ///
            /// Note that it's not an async start.
            void Start();
            /// @brief Stops the loop.
            void Stop();

            /// @brief Checks if the loop is running.
            bool IsRunning();

            /// @brief Schedules to call a function.
            ///
            /// Will not call if the Container is stopped before the call.
            ///
            /// @param Task The function that will be called.
            /// @param Time The time when the function will be called.
            ///        Time = 0 or Time <= CurrentTime results in calling the function shortly.
            /// @param Async Whether the call must be async.
            void Schedule(std::function<void()> Task, double Time = 0, bool Async = false);
            /// @brief Schedules to call a function.
            ///
            /// Will not call if the Container is stopped before the call.
            ///
            /// @param Task The function that will be called.
            /// @param Time The time when the function will be called.
            ///        Time = 0 or Time <= CurrentTime results in calling the function shortly.
            /// @param Async Whether the call must be async.
            void Schedule(double Time, std::function<void()> Task, bool Async = false);
            /// @brief Schedules to call a function.
            ///
            /// Will not call if the Container is stopped before the call.
            ///
            /// @param Task The function that will be called.
            /// @param Time The time when the function will be called.
            ///        Time = 0 or Time <= CurrentTime results in calling the function shortly.
            /// @param Async Whether the call must be async.
            void Schedule(double Time, bool Async, std::function<void()> Task);
        private:
            int ZeroPriorityModulesStartIndex;
            int ZeroPriorityModulesEndIndex;

            Data::Shared<bool> isRunning;
            Data::Shared<double> Time;
            Data::Shared<double> TimeDiff;
            Data::Shared<float> TimeFloat;
            Data::Shared<float> TimeDiffFloat;
            Data::Shared<bool> ShouldStop;

            Data::Collections::PriorityQueue<std::function<void()>, double> Schedules;
            Data::Collections::PriorityQueue<std::function<void()>, double> AsyncSchedules;
        };
    }
}
