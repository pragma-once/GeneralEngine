#ifndef ENGINE_PRIORITY_QUEUE_INCLUDED

#ifdef ENGINE_PRIORITY_QUEUE_USE_MUTEX
    #define ENGINE_PRIORITY_QUEUE_INCLUDED
#endif

#include "../../Engine.dec.h"
#include "ResizableArray.h"

#ifdef ENGINE_PRIORITY_QUEUE_USE_MUTEX
    #include "../MutexContained.h"
    #define ENGINE_PRIORITY_QUEUE_CLASS_NAME PriorityQueue<ItemsType, PriorityType, LessPriorityFirst, true>
    #define ENGINE_PRIORITY_QUEUE_DERIVATION : public MutexContained
#else
    #define ENGINE_PRIORITY_QUEUE_CLASS_NAME PriorityQueue<ItemsType, PriorityType, LessPriorityFirst, false>
    #define ENGINE_PRIORITY_QUEUE_DERIVATION
#endif

namespace Engine
{
    namespace Data
    {
        namespace Collections
        {
            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            class ENGINE_PRIORITY_QUEUE_CLASS_NAME final ENGINE_PRIORITY_QUEUE_DERIVATION
            {
#ifdef ENGINE_PRIORITY_QUEUE_USE_MUTEX
                friend PriorityQueue<ItemsType, PriorityType, LessPriorityFirst, false>;
#else
                friend PriorityQueue<ItemsType, PriorityType, LessPriorityFirst, true>;
#endif
            public:
                PriorityQueue(int InitialCapacity = 0);
                ~PriorityQueue();

                PriorityQueue(PriorityQueue<ItemsType, PriorityType, LessPriorityFirst, true>&) noexcept;
                PriorityQueue(PriorityQueue<ItemsType, PriorityType, LessPriorityFirst, true>&&) noexcept;
                PriorityQueue(PriorityQueue<ItemsType, PriorityType, LessPriorityFirst, false>&) noexcept;
                PriorityQueue(PriorityQueue<ItemsType, PriorityType, LessPriorityFirst, false>&&) noexcept;

                PriorityQueue& operator=(PriorityQueue<ItemsType, PriorityType, LessPriorityFirst, true>) noexcept;
                PriorityQueue& operator=(PriorityQueue<ItemsType, PriorityType, LessPriorityFirst, false>) noexcept;

                /// @brief Inserts an item to the back of its priority group.
                void Push(ItemsType Item, PriorityType Priority);
                /// @brief Pops the first item.
                /// @return The popped item.
                ItemsType Pop();
                /// @brief Pops the first item.
                /// @param ItemOut The popped item, if any.
                /// @return Whether there was an item to pop.
                bool Pop(ItemsType& ItemOut);
                /// @brief Pops the first item.
                /// @param ItemOut The popped item, if any.
                /// @param PriorityOut The popped item priority, if any.
                /// @return Whether there was an item to pop.
                bool Pop(ItemsType& ItemOut, PriorityType& PriorityOut);
                /// @brief Clears the priority queue.
                void Clear();

                /// @brief Expands the allocated memory.
                /// @param Space the space to add to the allocated memory.
                void Expand(int Space);
                /// @brief Shrinks the allocated memory.
                /// @param AdditionalSpace The space that must be left empty.
                ///        0 will shrink the space to fit the items.
                void Shrink(int AdditionalSpace = 0);
                /// @brief Sets whether the priority queue must shrink automatically on removing.
                ///
                /// Shrinking can be manually controlled in frequent push/pop situations.
                /// It is recommended to leave the AutoShrink on.
                void SetAutoShrink(bool Value);
                /// @brief Gets whether the priority queue shrinks automatically.
                ///
                /// Set the value using SetAutoShrink(bool).
                bool IsAutoShrink();

                /// @brief Gets the first item, without popping it.
                ItemsType GetFirstItem();
                /// @brief Gets the first item priority, without popping the item.
                PriorityType GetFirstPriority();
                /// @brief Gets the 0-based depth of the first matching item.
                /// @param Item The search subject.
                /// @param FromDepth The start depth for searching.
                /// @return The 0-based depth of the matching item if found,
                ///         -1 if no matching item found.
                int GetDepthOf(ItemsType Item, int FromDepth = 0);
                /// @brief Checks if a matching item exists in the priority queue.
                /// @param Item The search subject.
                /// @return True if the item was found once, else false.
                bool Contains(ItemsType Item);
                /// @brief Gets the items count.
                int GetCount();
                /// @brief Checks whether the priority queue is empty.
                bool IsEmpty();
                /// @brief Gets the current capacity of the allocated memory.
                int GetCapacity();
            private:
                ResizableArray<ItemsType, false> * ItemsRef;
                ResizableArray<PriorityType, false> * PrioritiesRef;
                int Count;
                bool AutoShrink;
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

                ItemsRef = new ResizableArray<ItemsType, false>(InitialCapacity);
                PrioritiesRef = new ResizableArray<PriorityType, false>(InitialCapacity);
                Count = 0;
                AutoShrink = true;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            ENGINE_PRIORITY_QUEUE_CLASS_NAME::~PriorityQueue()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                delete ItemsRef;
                delete PrioritiesRef;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            ENGINE_PRIORITY_QUEUE_CLASS_NAME::PriorityQueue(
                PriorityQueue<ItemsType, PriorityType, LessPriorityFirst, true>& Op
                ) noexcept : PriorityQueue()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                auto OpGuard = Op.Mutex.GetSharedLock();

                Count = Op.Count;
                *ItemsRef = *(Op.ItemsRef);
                *PrioritiesRef = *(Op.PrioritiesRef);
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            ENGINE_PRIORITY_QUEUE_CLASS_NAME::PriorityQueue(
                PriorityQueue<ItemsType, PriorityType, LessPriorityFirst, true>&& Op
                ) noexcept : PriorityQueue()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                auto OpGuard = Op.Mutex.GetSharedLock();

                std::swap(Count, Op.Count);
                std::swap(ItemsRef, Op.ItemsRef);
                std::swap(PrioritiesRef, Op.PrioritiesRef);
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            ENGINE_PRIORITY_QUEUE_CLASS_NAME::PriorityQueue(
                PriorityQueue<ItemsType, PriorityType, LessPriorityFirst, false>& Op
                ) noexcept : PriorityQueue()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                Count = Op.Count;
                *ItemsRef = *(Op.ItemsRef);
                *PrioritiesRef = *(Op.PrioritiesRef);
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            ENGINE_PRIORITY_QUEUE_CLASS_NAME::PriorityQueue(
                PriorityQueue<ItemsType, PriorityType, LessPriorityFirst, false>&& Op
                ) noexcept : PriorityQueue()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                std::swap(Count, Op.Count);
                std::swap(ItemsRef, Op.ItemsRef);
                std::swap(PrioritiesRef, Op.PrioritiesRef);
            }
            
            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            ENGINE_PRIORITY_QUEUE_CLASS_NAME& ENGINE_PRIORITY_QUEUE_CLASS_NAME::operator=(
                PriorityQueue<ItemsType, PriorityType, LessPriorityFirst, true> Op
                ) noexcept
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                auto OpGuard = Op.Mutex.GetSharedLock();

                std::swap(Count, Op.Count);
                std::swap(ItemsRef, Op.ItemsRef);
                std::swap(PrioritiesRef, Op.PrioritiesRef);

                return *this;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            ENGINE_PRIORITY_QUEUE_CLASS_NAME& ENGINE_PRIORITY_QUEUE_CLASS_NAME::operator=(
                PriorityQueue<ItemsType, PriorityType, LessPriorityFirst, false> Op
                ) noexcept
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                std::swap(Count, Op.Count);
                std::swap(ItemsRef, Op.ItemsRef);
                std::swap(PrioritiesRef, Op.PrioritiesRef);

                return *this;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            void ENGINE_PRIORITY_QUEUE_CLASS_NAME::Push(ItemsType Item, PriorityType Priority)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                while (Count >= ItemsRef->GetLength())
                    if (ItemsRef->GetLength() > 0)
                        ItemsRef->Resize(ItemsRef->GetLength() * 2);
                    else
                        ItemsRef->Resize(1);

                while (Count >= PrioritiesRef->GetLength())
                    if (PrioritiesRef->GetLength() > 0)
                        PrioritiesRef->Resize(PrioritiesRef->GetLength() * 2);
                    else
                        PrioritiesRef->Resize(1);

                int s = 0;
                int e = Count;
                while (s < e)
                {
                    int c = (s + e) / 2;
                    if (LessPriorityFirst)
                        if (Priority < PrioritiesRef->GetItem(c)) s = c + 1;
                        else e = c;
                    else
                        if (PrioritiesRef->GetItem(c) < Priority) s = c + 1;
                        else e = c;
                }

                for (int i = Count; i > s; i--)
                {
                    ItemsRef->SetItem(i, ItemsRef->GetItem(i - 1));
                    PrioritiesRef->SetItem(i, PrioritiesRef->GetItem(i - 1));
                }
                ItemsRef->SetItem(s, Item);
                PrioritiesRef->SetItem(s, Priority);
                Count++;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            ItemsType ENGINE_PRIORITY_QUEUE_CLASS_NAME::Pop()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Count <= 0)
                    throw std::logic_error("Cannot pop from an empty priority queue.");

                Count--;
                ItemsType Item = ItemsRef->GetItem(Count);

                if (AutoShrink && Count < ItemsRef->GetLength() / 2)
                    ItemsRef->Resize(ItemsRef->GetLength() / 2);

                return Item;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            bool ENGINE_PRIORITY_QUEUE_CLASS_NAME::Pop(ItemsType& ItemOut)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Count <= 0)
                    return false;

                Count--;
                ItemOut = ItemsRef->GetItem(Count);

                if (AutoShrink && Count < ItemsRef->GetLength() / 2)
                    ItemsRef->Resize(ItemsRef->GetLength() / 2);

                return true;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            bool ENGINE_PRIORITY_QUEUE_CLASS_NAME::Pop(ItemsType& ItemOut, PriorityType& PriorityOut)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Count <= 0)
                    return false;

                Count--;
                ItemOut = ItemsRef->GetItem(Count);
                PriorityOut = PrioritiesRef->GetItem(Count);

                if (AutoShrink && Count < ItemsRef->GetLength() / 2)
                    ItemsRef->Resize(ItemsRef->GetLength() / 2);

                return true;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            void ENGINE_PRIORITY_QUEUE_CLASS_NAME::Clear()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                Count = 0;
                if (AutoShrink)
                    ItemsRef->Resize(0);
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            void ENGINE_PRIORITY_QUEUE_CLASS_NAME::Expand(int Space)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Space < 0)
                    throw std::domain_error("Space is less than zero.");

                ItemsRef->Resize(ItemsRef->GetLength() + Space);
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            void ENGINE_PRIORITY_QUEUE_CLASS_NAME::Shrink(int AdditionalSpace)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (AdditionalSpace < 0)
                    throw std::domain_error("AdditionalSpace is less than zero.");

                ItemsRef->Resize(Count + AdditionalSpace);
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            void ENGINE_PRIORITY_QUEUE_CLASS_NAME::SetAutoShrink(bool Value)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                AutoShrink = Value;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            bool ENGINE_PRIORITY_QUEUE_CLASS_NAME::IsAutoShrink()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return AutoShrink;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            ItemsType ENGINE_PRIORITY_QUEUE_CLASS_NAME::GetFirstItem()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                if (Count <= 0)
                    throw std::logic_error("Cannot get the first item of an empty priority queue.");

                return ItemsRef->GetItem(Count - 1);
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            PriorityType ENGINE_PRIORITY_QUEUE_CLASS_NAME::GetFirstPriority()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                if (Count <= 0)
                    throw std::logic_error("Cannot get the first priority of an empty priority queue.");

                return PrioritiesRef->GetItem(Count - 1);
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            int ENGINE_PRIORITY_QUEUE_CLASS_NAME::GetDepthOf(ItemsType Item, int FromDepth)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = Count - 1 - FromDepth; i >= 0; i--)
                    if (ItemsRef->GetItem(i) == Item)
                        return (Count - 1) - i;

                return -1;
            }

            template <typename ItemsType, typename PriorityType, bool LessPriorityFirst>
            bool ENGINE_PRIORITY_QUEUE_CLASS_NAME::Contains(ItemsType Item)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = Count - 1; i >= 0; i--)
                    if (ItemsRef->GetItem(i) == Item)
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

                return ItemsRef->GetLength();
            }
        }
    }
}

#undef ENGINE_COLLECTION_WRITE_ACCESS
#undef ENGINE_COLLECTION_READ_ACCESS

#undef ENGINE_PRIORITY_QUEUE_CLASS_NAME
#undef ENGINE_PRIORITY_QUEUE_DERIVATION

#ifndef ENGINE_PRIORITY_QUEUE_USE_MUTEX
    #define ENGINE_PRIORITY_QUEUE_USE_MUTEX
    #include "PriorityQueue.h"
    #undef ENGINE_PRIORITY_QUEUE_USE_MUTEX
#endif

#endif // Include Guard
