#pragma once

#include "../Engine.dec.h"

namespace Engine
{
    namespace Core
    {
        class Container
        {
        public:
            const Data::Collections::List<Behavior*> * Behaviors;

            Container();
            ~Container();

            void Start();
            //void Update(); // only accessible through the thread that is running the behaviours, update all others. NO IT'S BAD! UPDATE INSIDE UPDATE CASE! any solutions?
            void End(bool EndNow = false);

            void Schedule(std::function<void()> Func, double Time = 0, bool Async = false);
            void Schedule(double Time, std::function<void()> Func, bool Async = false);
            void Schedule(double Time, bool Async, std::function<void()> Func);
        private:
            Data::Collections::PriorityQueue<std::function<void()>, double> * Schedules;
        };
    }
}