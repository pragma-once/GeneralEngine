#pragma once

namespace Engine
{
    namespace Data
    {
        namespace Collections
        {
            template <class ItemsType, bool UseMutex = true> class ResizableArray;
            template <class ItemsType, bool UseMutex = true> class List;
            template <class ItemsType, bool UseMutex = true> class ReadOnlyList;
            template <class ItemsType, bool UseMutex = true> class Stack;
            template <class ItemsType, bool UseMutex = true> class Queue;
            template <class ItemsType, class PriorityType = int, bool UseMutex = true> class PriorityQueue;
            template <class ValuesType, bool UseMutex = true> class Dictionary;
        }
    }
}