#pragma once

#include "../Engine.dec.h"
#include "SmartMutex.h"

namespace Engine
{
    namespace Data
    {
        class MutexContained
        {
        public:
            virtual ~MutexContained();
            /// @brief Calls the passed function while locking the object's mutex.
            void LockAndDo(std::function<void()> Process);
        protected:
            SmartMutex Mutex;
        };
    }
}
