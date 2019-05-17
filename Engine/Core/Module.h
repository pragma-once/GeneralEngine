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
            /// In the derived class, use this function to set the module priority
            /// to specify the execution order, using a constructor written like:
            ///     MyModule(...) : Module(MyPriority) {...}
            ///
            /// @param Priority Specifies modules execution order
            Module(std::int_fast8_t Priority);

            virtual ~Module();

            /// @brief Enables the module
            ///
            /// The OnUpdate will be called when the module is enabled
            /// and the container Loop is started.
            void Enable();
            /// @brief Disables the module
            ///
            /// The OnUpdate won't be called when the module is disabled.
            void Disable();

            /// @brief Checks whether the module is enabled.
            ///
            /// The module may not be running while enabled.
            bool IsEnabled();
            /// @brief Checks whether the module is running.
            ///
            /// Running means that the OnUpdate is being called.
            bool IsRunning();
            /// @brief Gets the module priority.
            ///
            /// Priority specifies modules execution order
            int GetPriority();

            virtual std::string GetName() = 0;
        protected:
            /// @brief Is called on loop start or when being added
            ///        to the loop while the loop is running.
            virtual void OnStart() = 0;
            /// @brief Is called on activation if the loop is running or just after OnStart.
            virtual void OnEnable() = 0;
            // @brief Synchronous update called by the loop.
            virtual void OnUpdate() = 0;
            /// @brief Is called on deactivation if the loop is running or just before OnStop.
            virtual void OnDisable() = 0;
            /// @brief Is called on loop stop or when being removed
            ///        from the loop while the loop is running.
            virtual void OnStop() = 0;

            /// @brief Gets the time since the Loop is started.
            double GetTime();
            /// @brief Gets the time between the last 2 updates.
            double GetTimeDiff();
            /// @brief Gets the time since the Loop is started as float.
            float GetTimeFloat();
            /// @brief Gets the time between the last 2 updates as float.
            float GetTimeDiffFloat();

            /// @brief Gets the loop which this module is added to.
            Loop * GetLoop();
        private:
            const std::int_fast8_t Priority;
            Data::Shared<bool> isEnabled;
            Data::Shared<Loop*> loop;

            void Acquire(Loop*);
            void Release();
            void _Start();
            void _Stop();
        };
    }
}
