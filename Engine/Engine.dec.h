#pragma once

#include <algorithm>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>

namespace Engine
{
    namespace Data
    {
        /// @brief Shared mutex that handles recursive locking in a thread.
        ///
        /// Try to use GetLock or GetSharedLock and avoid using Locking/Unlocking functions.
        class HandledMutex;
        /// @brief Object that uses a setter and/or a getter to be set or get.
        template <typename Type, bool HasSetter = true, bool HasGetter = true> class Property;
        /// @brief Shared object with automatic mutex locking on set/get
        template <typename Type> class Shared;

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
        /// @brief Manages and runs Module objects.
        class Container;
        /// @brief Abstract class to implement the application's modules.
        ///
        /// Add them to a Container to run.
        class Module;
    }
}
