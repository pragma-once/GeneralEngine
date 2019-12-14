#ifndef ENGINE_QUEUE_INCLUDED

#ifdef ENGINE_QUEUE_USE_MUTEX
    #define ENGINE_QUEUE_INCLUDED
#endif

#include "../../Engine.dec.h"
#include "ResizableArray.h"

#ifdef ENGINE_QUEUE_USE_MUTEX
    #include "../MutexContained.h"
    #define ENGINE_QUEUE_CLASS_NAME Queue<ItemsType, true>
    #define ENGINE_QUEUE_DERIVATION : public MutexContained<true, false>
#else
    #define ENGINE_QUEUE_CLASS_NAME Queue<ItemsType, false>
    #define ENGINE_QUEUE_DERIVATION
#endif

namespace Engine
{
    namespace Utilities
    {
        namespace Collections
        {
            template <typename ItemsType>
            class ENGINE_QUEUE_CLASS_NAME final ENGINE_QUEUE_DERIVATION
            {
#ifdef ENGINE_QUEUE_USE_MUTEX
                friend Queue<ItemsType, false>;
#else
                friend Queue<ItemsType, true>;
#endif
            public:
                Queue(int InitialCapacity = 0);
                ~Queue();

                Queue(Queue<ItemsType, true>&) noexcept;
                Queue(Queue<ItemsType, true>&&) noexcept;
                Queue(Queue<ItemsType, false>&) noexcept;
                Queue(Queue<ItemsType, false>&&) noexcept;

                Queue& operator=(Queue<ItemsType, true>) noexcept;
                Queue& operator=(Queue<ItemsType, false>) noexcept;

                /// @brief Pushes an item to the back.
                void Push(ItemsType Item);
                /// @brief Pops the first/front item.
                /// @return The popped item.
                ItemsType Pop();
                /// @brief Pops the first/front item.
                /// @param ItemOut The popped item, if any.
                /// @return Whether there was an item to pop.
                bool Pop(ItemsType& ItemOut);
                /// @brief Clears the queue.
                void Clear();

                /// @brief Expands the allocated memory.
                /// @param Space the space to add to the allocated memory.
                void Expand(int Space);
                /// @brief Shrinks the allocated memory.
                /// @param AdditionalSpace The space that must be left empty.
                ///        0 will shrink the space to fit the items.
                void Shrink(int AdditionalSpace = 0);
                /// @brief Sets whether the queue must shrink automatically on removing.
                ///
                /// Shrinking can be manually controlled in frequent push/pop situations.
                /// It is recommended to leave the AutoShrink on.
                void SetAutoShrink(bool Value);
                /// @brief Gets whether the queue shrinks automatically.
                ///
                /// Set the value using SetAutoShrink(bool).
                bool IsAutoShrink();

                /// @brief Gets the first item, without popping it.
                ItemsType GetFirst();
                /// @brief Gets the 0-based depth of the first matching item.
                /// @param Item The search subject.
                /// @param FromDepth The start depth for searching.
                /// @return The 0-based depth of the matching item if found,
                ///         -1 if no matching item found.
                int GetDepthOf(ItemsType Item, int FromDepth = 0);
                /// @brief Checks if a matching item exists in the queue.
                /// @param Item The search subject.
                /// @return True if the item was found once, else false.
                bool Contains(ItemsType Item);
                /// @brief Gets the items count.
                int GetCount();
                /// @brief Checks whether the queue is empty.
                bool IsEmpty();
                /// @brief Gets the current capacity of the allocated memory.
                int GetCapacity();
            private:
                ResizableArray<ItemsType, false> * ItemsRef;
                int First;
                int Count;
                bool AutoShrink;

                void Resize(int NewCapacity);
            };
        }
    }
}

// DEFINITION ----------------------------------------------------------------

#ifdef ENGINE_QUEUE_USE_MUTEX
    #define ENGINE_COLLECTION_WRITE_ACCESS auto guard = Mutex.GetLock();
    #define ENGINE_COLLECTION_READ_ACCESS auto guard = Mutex.GetSharedLock();
#else
    #define ENGINE_COLLECTION_WRITE_ACCESS ;
    #define ENGINE_COLLECTION_READ_ACCESS ;
#endif

namespace Engine
{
    namespace Utilities
    {
        namespace Collections
        {
            template <typename ItemsType>
            ENGINE_QUEUE_CLASS_NAME::Queue(int InitialCapacity)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                ItemsRef = new ResizableArray<ItemsType, false>(InitialCapacity);
                First = 0;
                Count = 0;
                AutoShrink = true;
            }

            template <typename ItemsType>
            ENGINE_QUEUE_CLASS_NAME::~Queue()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                delete ItemsRef;
            }

            template <typename ItemsType>
            ENGINE_QUEUE_CLASS_NAME::Queue(Queue<ItemsType, true>& Op) noexcept : Queue()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                auto OpGuard = Op.Mutex.GetSharedLock();

                First = Op.First;
                Count = Op.Count;
                *ItemsRef = *(Op.ItemsRef);
            }

            template <typename ItemsType>
            ENGINE_QUEUE_CLASS_NAME::Queue(Queue<ItemsType, true>&& Op) noexcept : Queue()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                auto OpGuard = Op.Mutex.GetSharedLock();

                std::swap(First, Op.First);
                std::swap(Count, Op.Count);
                std::swap(ItemsRef, Op.ItemsRef);
            }

            template <typename ItemsType>
            ENGINE_QUEUE_CLASS_NAME::Queue(Queue<ItemsType, false>& Op) noexcept : Queue()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                First = Op.First;
                Count = Op.Count;
                *ItemsRef = *(Op.ItemsRef);
            }

            template <typename ItemsType>
            ENGINE_QUEUE_CLASS_NAME::Queue(Queue<ItemsType, false>&& Op) noexcept : Queue()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                std::swap(First, Op.First);
                std::swap(Count, Op.Count);
                std::swap(ItemsRef, Op.ItemsRef);
            }

            template <typename ItemsType>
            ENGINE_QUEUE_CLASS_NAME& ENGINE_QUEUE_CLASS_NAME::operator=(Queue<ItemsType, true> Op) noexcept
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                auto OpGuard = Op.Mutex.GetSharedLock();

                std::swap(First, Op.First);
                std::swap(Count, Op.Count);
                std::swap(ItemsRef, Op.ItemsRef);

                return *this;
            }

            template <typename ItemsType>
            ENGINE_QUEUE_CLASS_NAME& ENGINE_QUEUE_CLASS_NAME::operator=(Queue<ItemsType, false> Op) noexcept
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                std::swap(First, Op.First);
                std::swap(Count, Op.Count);
                std::swap(ItemsRef, Op.ItemsRef);

                return *this;
            }

            template <typename ItemsType>
            void ENGINE_QUEUE_CLASS_NAME::Push(ItemsType Item)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                while (Count >= ItemsRef->GetLength())
                    if (ItemsRef->GetLength() > 0)
                        Resize(ItemsRef->GetLength() * 2);
                    else
                        Resize(1);

                ItemsRef->SetItem((First + Count) % ItemsRef->GetLength(), Item);
                Count++;
            }

            template <typename ItemsType>
            ItemsType ENGINE_QUEUE_CLASS_NAME::Pop()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Count <= 0)
                    throw std::logic_error("Cannot pop from an empty queue.");

                ItemsType Item = ItemsRef->GetItem(First);
                First = (First + 1) % ItemsRef->GetLength();
                Count--;

                if (AutoShrink && Count < ItemsRef->GetLength() / 2)
                    Resize(ItemsRef->GetLength() / 2);

                return Item;
            }

            template <typename ItemsType>
            bool ENGINE_QUEUE_CLASS_NAME::Pop(ItemsType& ItemOut)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Count <= 0)
                    return false;

                ItemOut = ItemsRef->GetItem(First);
                First = (First + 1) % ItemsRef->GetLength();
                Count--;

                if (AutoShrink && Count < ItemsRef->GetLength() / 2)
                    Resize(ItemsRef->GetLength() / 2);

                return true;
            }

            template <typename ItemsType>
            void ENGINE_QUEUE_CLASS_NAME::Clear()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                First = 0;
                Count = 0;
                if (AutoShrink)
                    ItemsRef->Resize(0);
            }

            template <typename ItemsType>
            void ENGINE_QUEUE_CLASS_NAME::Expand(int Space)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Space < 0)
                    throw std::domain_error("Space is less than zero.");

                Resize(ItemsRef->GetLength() + Space);
            }

            template <typename ItemsType>
            void ENGINE_QUEUE_CLASS_NAME::Shrink(int AdditionalSpace)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (AdditionalSpace < 0)
                    throw std::domain_error("AdditionalSpace is less than zero.");

                Resize(Count + AdditionalSpace);
            }

            template <typename ItemsType>
            void ENGINE_QUEUE_CLASS_NAME::SetAutoShrink(bool Value)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                AutoShrink = Value;
            }

            template <typename ItemsType>
            bool ENGINE_QUEUE_CLASS_NAME::IsAutoShrink()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return AutoShrink;
            }

            template <typename ItemsType>
            ItemsType ENGINE_QUEUE_CLASS_NAME::GetFirst()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                if (Count <= 0)
                    throw std::logic_error("Cannot get the first element of an empty queue.");

                return ItemsRef->GetItem(First);
            }

            template <typename ItemsType>
            int ENGINE_QUEUE_CLASS_NAME::GetDepthOf(ItemsType Item, int FromDepth)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                if (Count == 0)
                    return -1;

                int Last = (First + Count - 1) % ItemsRef->GetLength();
                int From = (First + FromDepth) % ItemsRef->GetLength();

                if (From <= Last)
                {
                    for (int i = From; i <= Last; i++)
                        if (ItemsRef->GetItem(i) == Item)
                            return i - First;
                }
                else
                {
                    for (int i = From; i < ItemsRef->GetLength(); i++)
                        if (ItemsRef->GetItem(i) == Item)
                            return i - First;
                    for (int i = 0; i <= Last; i++)
                        if (ItemsRef->GetItem(i) == Item)
                            return (ItemsRef->GetLength() - First) + i;
                }

                return -1;
            }

            template <typename ItemsType>
            bool ENGINE_QUEUE_CLASS_NAME::Contains(ItemsType Item)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                if (Count == 0)
                    return false;

                int Last = (First + Count - 1) % ItemsRef->GetLength();

                if (First <= Last)
                {
                    for (int i = First; i <= Last; i++)
                        if (ItemsRef->GetItem(i) == Item)
                            return true;
                }
                else
                {
                    for (int i = First; i < ItemsRef->GetLength(); i++)
                        if (ItemsRef->GetItem(i) == Item)
                            return true;
                    for (int i = 0; i <= Last; i++)
                        if (ItemsRef->GetItem(i) == Item)
                            return true;
                }

                return false;
            }

            template <typename ItemsType>
            int ENGINE_QUEUE_CLASS_NAME::GetCount()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return Count;
            }

            template <typename ItemsType>
            bool ENGINE_QUEUE_CLASS_NAME::IsEmpty()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return Count == 0;
            }

            template <typename ItemsType>
            int ENGINE_QUEUE_CLASS_NAME::GetCapacity()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return ItemsRef->GetLength();
            }

            template <typename ItemsType>
            void ENGINE_QUEUE_CLASS_NAME::Resize(int NewCapacity)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (First == 0 || Count == 0)
                    ItemsRef->Resize(NewCapacity);
                else
                {
                    ResizableArray<ItemsType, false> * PrevItems = ItemsRef;
                    int prev_capacity = PrevItems->GetLength();
                    ItemsRef = new ResizableArray<ItemsType, false>(NewCapacity);
                    for (int i = 0; i < Count; i++)
                        ItemsRef->SetItem(i, PrevItems->GetItem((First + i) % prev_capacity));
                    First = 0;
                    delete PrevItems;
                }
            }
        }
    }
}

#undef ENGINE_COLLECTION_WRITE_ACCESS
#undef ENGINE_COLLECTION_READ_ACCESS

#undef ENGINE_QUEUE_CLASS_NAME
#undef ENGINE_QUEUE_DERIVATION

#ifndef ENGINE_QUEUE_USE_MUTEX
    #define ENGINE_QUEUE_USE_MUTEX
    #include "Queue.h"
    #undef ENGINE_QUEUE_USE_MUTEX
#endif

#endif // Include Guard
