#ifndef ENGINE_PRIORITY_QUEUE_INCLUDED

#ifdef ENGINE_PRIORITY_QUEUE_USE_MUTEX
    #define ENGINE_PRIORITY_QUEUE_INCLUDED
#endif

#include "../../Engine.dec.h"
#include "ResizableArray.h"

#ifdef ENGINE_PRIORITY_QUEUE_USE_MUTEX
    #include "../HandledMutex.h"
    #define ENGINE_PRIORITY_QUEUE_CLASS_NAME PriorityQueue<ItemsType, PriorityType, LessPriorityFirst, true>
#else
    #define ENGINE_PRIORITY_QUEUE_CLASS_NAME PriorityQueue<ItemsType, PriorityType, LessPriorityFirst, false>
#endif

namespace Engine
{
    namespace Data
    {
        namespace Collections
        {
            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            class ENGINE_PRIORITY_QUEUE_CLASS_NAME
            {
            public:
                PriorityQueue(int InitialCapacity = 0);
                ~PriorityQueue();

                PriorityQueue(const PriorityQueue<ItemsType, PriorityType, true>&);
                PriorityQueue& operator=(const PriorityQueue<ItemsType, PriorityType, true>&);
                PriorityQueue(const PriorityQueue<ItemsType, PriorityType, false>&);
                PriorityQueue& operator=(const PriorityQueue<ItemsType, PriorityType, false>&);

                void Push(ItemsType Item, PriorityType Priority);
                ItemsType Pop();
                bool Pop(ItemsType& ItemOut);
                bool Pop(ItemsType& ItemOut, PriorityType& PriorityOut);
                void Clear();

                void Expand(int Space);
                void Shrink(int AdditionalSpace = 0);

                ItemsType GetFirstItem();
                PriorityType GetFirstPriority();
                int GetDepthOf(ItemsType Item);
                bool Contains(ItemsType Item);
                int GetCount();
                bool IsEmpty();
                int GetCapacity();
            private:
#ifdef ENGINE_PRIORITY_QUEUE_USE_MUTEX
                HandledMutex Mutex;
#endif
                int Count;
                ResizableArray<ItemsType, false> * Items;
                ResizableArray<PriorityType, false> * Priorities;
            };
        }
    }
}

// DEFINITION ----------------------------------------------------------------

#ifdef ENGINE_PRIORITY_QUEUE_USE_MUTEX
    #define ENGINE_COLLECTION_WRITE_ACCESS auto guard = Mutex.GetLock();
    #define ENGINE_COLLECTION_READ_ACCESS auto guard = Mutex.GetSharedLock();
#else
    #define ENGINE_COLLECTION_WRITE_ACCESS ;
    #define ENGINE_COLLECTION_READ_ACCESS ;
#endif

namespace Engine
{
    namespace Data
    {
        namespace Collections
        {
            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            ENGINE_PRIORITY_QUEUE_CLASS_NAME::PriorityQueue(int InitialCapacity)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                Items = new ResizableArray<ItemsType, false>(InitialCapacity);
                Priorities = new ResizableArray<PriorityType, false>(InitialCapacity);
                Count = 0;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            ENGINE_PRIORITY_QUEUE_CLASS_NAME::~PriorityQueue()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                delete Items;
                delete Priorities;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            ENGINE_PRIORITY_QUEUE_CLASS_NAME::PriorityQueue(const PriorityQueue<ItemsType, PriorityType, true>& Op)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                auto OpGuard = Op.Mutex.GetSharedLock();

                Count = Op.Count;
                *Items = *(Op.Items);
                *Priorities = *(Op.Priorities);
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            ENGINE_PRIORITY_QUEUE_CLASS_NAME& ENGINE_PRIORITY_QUEUE_CLASS_NAME::operator=(const PriorityQueue<ItemsType, PriorityType, true>& Op)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                auto OpGuard = Op.Mutex.GetSharedLock();

                Count = Op.Count;
                *Items = *(Op.Items);
                *Priorities = *(Op.Priorities);

                return *this;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            ENGINE_PRIORITY_QUEUE_CLASS_NAME::PriorityQueue(const PriorityQueue<ItemsType, PriorityType, false>& Op)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                Count = Op.Count;
                *Items = *(Op.Items);
                *Priorities = *(Op.Priorities);
            }
            
            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            ENGINE_PRIORITY_QUEUE_CLASS_NAME& ENGINE_PRIORITY_QUEUE_CLASS_NAME::operator=(const PriorityQueue<ItemsType, PriorityType, false>& Op)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                Count = Op.Count;
                *Items = *(Op.Items);
                *Priorities = *(Op.Priorities);

                return *this;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            void ENGINE_PRIORITY_QUEUE_CLASS_NAME::Push(ItemsType Item, PriorityType Priority)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                while (Count >= Items->GetLength())
                    if (Items->GetLength() > 0)
                        Items->Resize(Items->GetLength() * 2);
                    else
                        Items->Resize(1);

                while (Count >= Priorities->GetLength())
                    if (Priorities->GetLength() > 0)
                        Priorities->Resize(Priorities->GetLength() * 2);
                    else
                        Priorities->Resize(1);

                int s = 0;
                int e = Count;
                while (s < e)
                {
                    int c = (s + e) / 2;
                    if (LessPriorityFirst)
                        if (Priority < Priorities->GetItem(c)) s = c + 1;
                        else e = c;
                    else
                        if (Priorities->GetItem(c) < Priority) s = c + 1;
                        else e = c;
                }

                for (int i = Count; i > s; i--)
                {
                    Items->SetItem(i, Items->GetItem(i - 1));
                    Priorities->SetItem(i, Priorities->GetItem(i - 1));
                }
                Items->SetItem(s, Item);
                Priorities->SetItem(s, Priority);
                Count++;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            ItemsType ENGINE_PRIORITY_QUEUE_CLASS_NAME::Pop()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Count <= 0)
                    throw std::logic_error("Cannot pop from an empty priority queue.");

                Count--;
                ItemsType Item = Items->GetItem(Count);

                if (Count < Items->GetLength() / 2)
                    Items->Resize(Items->GetLength() / 2);

                return Item;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            bool ENGINE_PRIORITY_QUEUE_CLASS_NAME::Pop(ItemsType& ItemOut)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Count <= 0)
                    return false;

                Count--;
                ItemOut = Items->GetItem(Count);

                if (Count < Items->GetLength() / 2)
                    Items->Resize(Items->GetLength() / 2);

                return true;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            bool ENGINE_PRIORITY_QUEUE_CLASS_NAME::Pop(ItemsType& ItemOut, PriorityType& PriorityOut)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Count <= 0)
                    return false;

                Count--;
                ItemOut = Items->GetItem(Count);
                PriorityOut = Priorities->GetItem(Count);

                if (Count < Items->GetLength() / 2)
                    Items->Resize(Items->GetLength() / 2);

                return true;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            void ENGINE_PRIORITY_QUEUE_CLASS_NAME::Clear()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                Count = 0;
                Items->Resize(0);
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            void ENGINE_PRIORITY_QUEUE_CLASS_NAME::Expand(int Space)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Space < 0)
                    throw std::domain_error("Space is less than zero.");

                Items->Resize(Items->GetLength() + Space);
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            void ENGINE_PRIORITY_QUEUE_CLASS_NAME::Shrink(int AdditionalSpace)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (AdditionalSpace < 0)
                    throw std::domain_error("AdditionalSpace is less than zero.");

                Items->Resize(Count + AdditionalSpace);
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            ItemsType ENGINE_PRIORITY_QUEUE_CLASS_NAME::GetFirstItem()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                if (Count <= 0)
                    throw std::logic_error("Cannot get the first item of an empty priority queue.");

                return Items->GetItem(Count - 1);
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            PriorityType ENGINE_PRIORITY_QUEUE_CLASS_NAME::GetFirstPriority()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                if (Count <= 0)
                    throw std::logic_error("Cannot get the first priority of an empty priority queue.");

                return Priorities->GetItem(Count - 1);
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            int ENGINE_PRIORITY_QUEUE_CLASS_NAME::GetDepthOf(ItemsType Item)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = Count - 1; i >= 0; i--)
                    if (Items->GetItem(i) == Item)
                        return (Count - 1) - i;

                return -1;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            bool ENGINE_PRIORITY_QUEUE_CLASS_NAME::Contains(ItemsType Item)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = Count - 1; i >= 0; i--)
                    if (Items->GetItem(i) == Item)
                        return true;

                return false;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            int ENGINE_PRIORITY_QUEUE_CLASS_NAME::GetCount()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return Count;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            bool ENGINE_PRIORITY_QUEUE_CLASS_NAME::IsEmpty()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return Count == 0;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            int ENGINE_PRIORITY_QUEUE_CLASS_NAME::GetCapacity()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return Items->GetLength();
            }
        }
    }
}

#undef ENGINE_COLLECTION_WRITE_ACCESS
#undef ENGINE_COLLECTION_READ_ACCESS

#undef ENGINE_PRIORITY_QUEUE_CLASS_NAME

#ifndef ENGINE_PRIORITY_QUEUE_USE_MUTEX
    #define ENGINE_PRIORITY_QUEUE_USE_MUTEX
    #include "PriorityQueue.h"
    #undef ENGINE_PRIORITY_QUEUE_USE_MUTEX
#endif

#endif // Include Guard
