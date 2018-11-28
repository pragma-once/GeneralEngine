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
            friend Behavior;
        public:
            Data::Collections::List<Behavior*> Behaviors;

            Container();

            void Start();
            void End(bool EndNow = false);

            bool IsRunning();

            void Schedule(std::function<void()> Func, double Time = 0, bool Async = false);
            void Schedule(double Time, std::function<void()> Func, bool Async = false);
            void Schedule(double Time, bool Async, std::function<void()> Func);
        private:
            int ZeroPriorityBehaviorsStartIndex;
            int ZeroPriorityBehaviorsEndIndex;

            Data::Shared<bool> isRunning;
            Data::Shared<double> Time;
            Data::Shared<double> TimeDiff;
            Data::Shared<float> TimeFloat;
            Data::Shared<float> TimeDiffFloat;
            Data::Shared<bool> ShouldEnd;

            Data::Collections::PriorityQueue<std::function<void()>, double> Schedules;
            Data::Collections::PriorityQueue<std::function<void()>, double> AsyncSchedules;
        };
    }
}
