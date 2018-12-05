#pragma once

#include "../Engine.dec.h"
#include "../Data/Shared.h"

namespace Engine
{
    namespace Core
    {
        class Module
        {
            friend Container;
        public:
            Module();
            Module(int Priority);

            void Activate();
            void Deactivate();

            bool IsActive();
            bool IsRunning();
            int GetPriority();

            virtual std::string GetName() = 0;
        protected:
            virtual void Start() = 0;
            virtual void Update() = 0;
            virtual void End() = 0;
            virtual void OnActivate() = 0;
            virtual void OnDeactivate() = 0;

            double GetTime();
            double GetTimeDiff();
            float GetTimeFloat();
            float GetTimeDiffFloat();

            Container * GetContainer();
        private:
            const int Priority;
            Data::Shared<bool> isActive;
            Data::Shared<Container*> container;

            void Acquire(Container*);
            void Release();
            void _Start();
            void _End();
        };
    }
}
