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

            double GetTime();
            double GetTimeDiff();
            float GetTimeFloat();
            float GetTimeDiffFloat();

            virtual int GetPriority();
        protected:
            virtual void Start() = 0;
            virtual void Update() = 0;
            virtual void End() = 0;
            virtual void OnActivate() = 0;
            virtual void OnDeactivate() = 0;
            virtual std::string GetName() = 0;
        private:
            Container * Owner;
        };
    }
}