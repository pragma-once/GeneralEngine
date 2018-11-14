#pragma once

#include "../Engine.dec.h"

namespace Engine
{
    namespace Core
    {
        class Behavior // TODO: Rename
        {
            friend Container;
        public:
            Behavior();
            Behavior(int Priority);

            void Activate();
            void Deactivate();

            int GetPriority(); // TODO: Rename Priority, including: Container's member variables, Container::Behaviors::OnRemove's local variable
        protected:
            virtual void Start() = 0;
            virtual void Update() = 0;
            virtual void End() = 0;
            virtual void OnActivate() = 0;
            virtual void OnDeactivate() = 0;
            virtual std::string GetName() = 0;

            double GetTime();
            double GetTimeDiff();
            float GetTimeFloat();
            float GetTimeDiffFloat();
        private:
            Container * Owner;
            const int Priority;
        };
    }
}