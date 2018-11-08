#pragma once

#include "../Engine.dec.h"

namespace Engine
{
    namespace Core
    {
        class Behavior
        {
            friend Container;
        public:
            virtual int GetPriority();
        protected:
            virtual void Start() = 0;
            virtual void Update() = 0;
            virtual void End() = 0;
            virtual void OnActive() = 0;
            virtual void OnDeactive() = 0;
            std::string GetName() = 0;
        };
    }
}