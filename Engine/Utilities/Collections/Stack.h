#ifndef ENGINE_STACK_INCLUDED

#ifdef ENGINE_STACK_USE_MUTEX
    #define ENGINE_STACK_INCLUDED
#endif

#include "../../Engine.dec.h"
#include "ResizableArray.h"

#ifdef ENGINE_STACK_USE_MUTEX
    #include "../MutexContained.h"
    #define ENGINE_STACK_CLASS_NAME Stack<ItemsType, true>
    #define ENGINE_STACK_DERIVATION : public MutexContained<true, false>
#else
    #define ENGINE_STACK_CLASS_NAME Stack<ItemsType, false>
    #define ENGINE_STACK_DERIVATION
#endif

namespace Engine
{
    namespace Utilities
    {
        namespace Collections
        {
            template <typename ItemsType>
            class ENGINE_STACK_CLASS_NAME final ENGINE_STACK_DERIVATION
            {
#ifdef ENGINE_STACK_USE_MUTEX
                friend Stack<ItemsType, false>;
#else
                friend Stack<ItemsType, true>;
#endif
            public:
                Stack(int InitialCapacity = 0);
                ~Stack();

                Stack(Stack<ItemsType, true>&) noexcept;
                Stack(Stack<ItemsType, true>&&) noexcept;
                Stack(Stack<ItemsType, false>&) noexcept;
                Stack(Stack<ItemsType, false>&&) noexcept;

                Stack& operator=(Stack<ItemsType, true>) noexcept;
                Stack& operator=(Stack<ItemsType, false>) noexcept;

                /// @brief Pushes an item to the top.
                void Push(ItemsType Item);
                /// @brief Pops the top item.
                /// @return The popped item.
                ItemsType Pop();
                /// @brief Pops the top item.
                /// @param ItemOut The popped item, if any.
                /// @return Whether there was an item to pop.
                bool Pop(ItemsType& ItemOut);
                /// @brief Sets the top item.
                void SetTop(ItemsType Value);
                /// @brief Clears the stack.
                void Clear();

                /// @brief Expands the allocated memory.
                /// @param Space the space to add to the allocated memory.
                void Expand(int Space);
                /// @brief Shrinks the allocated memory.
                /// @param AdditionalSpace The space that must be left empty.
                ///        0 will shrink the space to fit the items.
                void Shrink(int AdditionalSpace = 0);
                /// @brief Sets whether the stack must shrink automatically on removing.
                ///
                /// Shrinking can be manually controlled in frequent push/pop situations.
                /// It is recommended to leave the AutoShrink on.
                void SetAutoShrink(bool Value);
                /// @brief Gets whether the stack shrinks automatically.
                ///
                /// Set the value using SetAutoShrink(bool).
                bool IsAutoShrink();

                /// @brief Gets the top item, without popping it.
                ItemsType GetTop();
                /// @brief Gets the 0-based depth of the top matching item.
                /// @param Item The search subject.
                /// @param FromDepth The start depth for searching.
                /// @return The 0-based depth of the matching item if found,
                ///         -1 if no matching item found.
                int GetDepthOf(ItemsType Item, int FromDepth = 0);
                /// @brief Checks if a matching item exists in the stack.
                /// @param Item The search subject.
                /// @return True if the item was found once, else false.
                bool Contains(ItemsType Item);
                /// @brief Gets the items count.
                int GetCount();
                /// @brief Checks whether the stack is empty.
                bool IsEmpty();
                /// @brief Gets the current capacity of the allocated memory.
                int GetCapacity();
            private:
                ResizableArray<ItemsType, false> * ItemsRef;
                int Count;
                bool AutoShrink;
            };
        }
    }
}

// DEFINITION ----------------------------------------------------------------

#ifdef ENGINE_STACK_USE_MUTEX
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
            ENGINE_STACK_CLASS_NAME::Stack(int InitialCapacity)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                ItemsRef = new ResizableArray<ItemsType, false>(InitialCapacity);
                Count = 0;
                AutoShrink = true;
            }

            template <typename ItemsType>
            ENGINE_STACK_CLASS_NAME::~Stack()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                delete ItemsRef;
            }

            template <typename ItemsType>
            ENGINE_STACK_CLASS_NAME::Stack(Stack<ItemsType, true>& Op) noexcept : Stack()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                auto OpGuard = Op.Mutex.GetSharedLock();

                Count = Op.Count;
                *ItemsRef = *(Op.ItemsRef);
            }

            template <typename ItemsType>
            ENGINE_STACK_CLASS_NAME::Stack(Stack<ItemsType, true>&& Op) noexcept : Stack()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                auto OpGuard = Op.Mutex.GetSharedLock();

                std::swap(Count, Op.Count);
                std::swap(ItemsRef, Op.ItemsRef);
            }

            template <typename ItemsType>
            ENGINE_STACK_CLASS_NAME::Stack(Stack<ItemsType, false>& Op) noexcept : Stack()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                Count = Op.Count;
                *ItemsRef = *(Op.ItemsRef);
            }

            template <typename ItemsType>
            ENGINE_STACK_CLASS_NAME::Stack(Stack<ItemsType, false>&& Op) noexcept : Stack()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                std::swap(Count, Op.Count);
                std::swap(ItemsRef, Op.ItemsRef);
            }

            template <typename ItemsType>
            ENGINE_STACK_CLASS_NAME& ENGINE_STACK_CLASS_NAME::operator=(Stack<ItemsType, true> Op) noexcept
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                auto OpGuard = Op.Mutex.GetSharedLock();

                std::swap(Count, Op.Count);
                std::swap(ItemsRef, Op.ItemsRef);

                return *this;
            }

            template <typename ItemsType>
            ENGINE_STACK_CLASS_NAME& ENGINE_STACK_CLASS_NAME::operator=(Stack<ItemsType, false> Op) noexcept
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                std::swap(Count, Op.Count);
                std::swap(ItemsRef, Op.ItemsRef);

                return *this;
            }

            template <typename ItemsType>
            void ENGINE_STACK_CLASS_NAME::Push(ItemsType Item)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                while (Count >= ItemsRef->GetLength())
                    if (ItemsRef->GetLength() > 0)
                        ItemsRef->Resize(ItemsRef->GetLength() * 2);
                    else
                        ItemsRef->Resize(1);

                ItemsRef->SetItem(Count, Item);
                Count++;
            }

            template <typename ItemsType>
            ItemsType ENGINE_STACK_CLASS_NAME::Pop()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Count <= 0)
                    throw std::logic_error("Cannot pop from an empty stack.");

                Count--;
                ItemsType Item = ItemsRef->GetItem(Count);

                if (AutoShrink && Count < ItemsRef->GetLength() / 2)
                    ItemsRef->Resize(ItemsRef->GetLength() / 2);

                return Item;
            }

            template <typename ItemsType>
            bool ENGINE_STACK_CLASS_NAME::Pop(ItemsType& ItemOut)
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

            template <typename ItemsType>
            void ENGINE_STACK_CLASS_NAME::SetTop(ItemsType Value)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Count <= 0)
                    throw std::logic_error("Cannot set the top element of an empty stack.");

                ItemsRef->SetItem(Count - 1, Value);
            }

            template <typename ItemsType>
            void ENGINE_STACK_CLASS_NAME::Clear()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                Count = 0;
                if (AutoShrink)
                    ItemsRef->Resize(0);
            }

            template <typename ItemsType>
            void ENGINE_STACK_CLASS_NAME::Expand(int Space)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Space < 0)
                    throw std::domain_error("Space is less than zero.");

                ItemsRef->Resize(ItemsRef->GetLength() + Space);
            }

            template <typename ItemsType>
            void ENGINE_STACK_CLASS_NAME::Shrink(int AdditionalSpace)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (AdditionalSpace < 0)
                    throw std::domain_error("AdditionalSpace is less than zero.");

                ItemsRef->Resize(Count + AdditionalSpace);
            }

            template <typename ItemsType>
            void ENGINE_STACK_CLASS_NAME::SetAutoShrink(bool Value)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                AutoShrink = Value;
            }

            template <typename ItemsType>
            bool ENGINE_STACK_CLASS_NAME::IsAutoShrink()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return AutoShrink;
            }

            template <typename ItemsType>
            ItemsType ENGINE_STACK_CLASS_NAME::GetTop()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                if (Count <= 0)
                    throw std::logic_error("Cannot get the top element of an empty stack.");

                return ItemsRef->GetItem(Count - 1);
            }

            template <typename ItemsType>
            int ENGINE_STACK_CLASS_NAME::GetDepthOf(ItemsType Item, int FromDepth)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = Count - 1 - FromDepth; i >= 0; i--)
                    if (ItemsRef->GetItem(i) == Item)
                        return (Count - 1) - i;

                return -1;
            }

            template <typename ItemsType>
            bool ENGINE_STACK_CLASS_NAME::Contains(ItemsType Item)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = Count - 1; i >= 0; i--)
                    if (ItemsRef->GetItem(i) == Item)
                        return true;

                return false;
            }

            template <typename ItemsType>
            int ENGINE_STACK_CLASS_NAME::GetCount()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return Count;
            }

            template <typename ItemsType>
            bool ENGINE_STACK_CLASS_NAME::IsEmpty()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return Count == 0;
            }

            template <typename ItemsType>
            int ENGINE_STACK_CLASS_NAME::GetCapacity()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return ItemsRef->GetLength();
            }
        }
    }
}

#undef ENGINE_COLLECTION_WRITE_ACCESS
#undef ENGINE_COLLECTION_READ_ACCESS

#undef ENGINE_STACK_CLASS_NAME
#undef ENGINE_STACK_DERIVATION

#ifndef ENGINE_STACK_USE_MUTEX
    #define ENGINE_STACK_USE_MUTEX
    #include "Stack.h"
    #undef ENGINE_STACK_USE_MUTEX
#endif

#endif // Include Guard
