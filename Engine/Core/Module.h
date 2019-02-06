#pragma once

#include "../Engine.dec.h"
#include "../Data/Shared.h"

namespace Engine
{
    namespace Core
    {
        class Module
        {
            friend Loop;
        public:
            Module();
            Module(int Priority);

            virtual ~Module();

            void Activate();
            void Deactivate();

            bool IsActive();
            bool IsRunning();
            int GetPriority();

            virtual std::string GetName() = 0;
        protected:
            virtual void OnStart() = 0;
            virtual void OnActivate() = 0;
            virtual void OnUpdate() = 0;
            virtual void OnDeactivate() = 0;
            virtual void OnStop() = 0;

            double GetTime();
            double GetTimeDiff();
            float GetTimeFloat();
            float GetTimeDiffFloat();

            Loop * GetLoop();
        private:
            const int Priority;
            Data::Shared<bool> isActive;
            Data::Shared<Loop*> loop;

            void Acquire(Loop*);
            void Release();
            void _Start();
            void _Stop();
        };
    }
}
