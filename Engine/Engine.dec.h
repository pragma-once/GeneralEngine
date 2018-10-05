#pragma once

#include <functional>
#include <mutex>
#include <shared_mutex>

namespace Engine
{
    namespace Data
    {
        namespace Collections
        {
            template <typename ItemsType, bool UseMutex = true> class ResizableArray;
            template <typename ItemsType, bool UseMutex = true> class List;
            template <typename ItemsType, bool UseMutex = true> class ReadOnlyList;
            template <typename ItemsType, bool UseMutex = true> class Stack;
            template <typename ItemsType, bool UseMutex = true> class Queue;
            template <typename ItemsType, class PriorityType = int, bool UseMutex = true> class PriorityQueue;
            template <typename ValuesType, bool UseMutex = true> class Dictionary;
        }
    }
}