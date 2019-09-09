#pragma once

#include <algorithm>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <utility>

namespace Engine
{
    namespace Utilities
    {
        /// @brief Shared mutex that handles recursive locking in a thread.
        ///
        /// This class is meant for sharing data between different threads
        /// and is much easier to use than a normal shared mutex.
        /// Try to use GetLock or GetSharedLock and avoid using Locking/Unlocking functions.
        class SmartMutex;
        /// @brief To be the base class for the classes that use a SmartMutex
        ///        and need a LockAndDo function.
        class MutexContained;
        /// @brief Shared object with automatic mutex locking on set/get.
        /// @tparam AllowManualLocking If true, The class will use a public SmartMutex that
        ///         can also be controlled by user.
        ///         Else, a private std::shared_mutex will be used.
        template <typename Type, bool AllowManualLocking = false> class Shared;
        /// @brief Provides objects that act on set and/or get.
        template <typename Type, bool SetAllowed = true, bool GetAllowed = true> class Accessor;

        namespace Collections
        {
            template <typename ItemsType, bool UseMutex = true> class ResizableArray;
            /// @brief Array list or array list interface.
            ///
            /// List interfaces define custom Add, Remove, SetItem and Clear functions for a list.
            template <typename ItemsType, bool UseMutex = true> class List;
            template <typename ItemsType, bool UseMutex = true> class Stack;
            template <typename ItemsType, bool UseMutex = true> class Queue;
            template <typename ItemsType, typename PriorityType = int, bool LessPriorityFirst = true, bool UseMutex = true> class PriorityQueue;
            template <typename KeyType, typename ValueType, bool UseMutex = true> class Dictionary;
        }
    }

    namespace Core
    {
        /// @brief The execution type of a Module or a Scheduled task in a Loop
        enum ExecutionType : std::int_fast8_t {
            /// @brief Wait for and block all BoundedAsync processes to execute.
            ///        May be used for core modules that really need this.
            ///        TRY NOT TO use this for 0-priority modules
            ///        as it will be wasting more time on updates.
            SingleThreaded = -1,
            /// @brief Are executed inside the loop in separate threads.
            ///        Recommended for normal Modules.
            BoundedAsync = 0,
            /// @brief Are executed outside the loop in new individual threads.
            ///        Only recommended for long scheduled tasks that sleep a lot,
            ///        and NOT Modules.
            FreeAsync = 1,
        };
        /// @brief Manages and runs Module objects.
        class Loop;
        /// @brief Abstract class to implement the application's modules.
        ///
        /// Add them to a Loop to run.
        class Module;
    }
}
