#ifndef ENGINE_STACK_INCLUDED

#ifdef ENGINE_STACK_USE_MUTEX
    #define ENGINE_STACK_INCLUDED
#endif

#include "../../Engine.dec.h"
#include "ResizableArray.h"

#ifdef ENGINE_STACK_USE_MUTEX
    #include "../HandledMutex.h"
    #define ENGINE_STACK_CLASS_NAME Stack<ItemsType, true>
#else
    #define ENGINE_STACK_CLASS_NAME Stack<ItemsType, false>
#endif

namespace Engine
{
    namespace Data
    {
        namespace Collections
        {
            template <typename ItemsType>
            class ENGINE_STACK_CLASS_NAME
            {
            public:
                Stack(const Stack&) = delete;
                Stack& operator=(const Stack&) = delete;

                Stack(int InitialCapacity = 0);
                ~Stack();

                void Push(ItemsType Item);
                ItemsType Pop();
                bool Pop(ItemsType& ItemOut);
                void SetTop(ItemsType Value);
                void Clear();

                void Expand(int Space);
                void Shrink(int AdditionalSpace = 0);

                ItemsType GetTop();
                int GetDepthOf(ItemsType Item);
                bool Exists(ItemsType Item);
                int GetCount();
                bool IsEmpty();
                int GetCapacity();
            private:
#ifdef ENGINE_STACK_USE_MUTEX
                HandledMutex Mutex;
#endif
                int Count;
                ResizableArray<ItemsType, false> * Items;
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
    namespace Data
    {
        namespace Collections
        {
            template <typename ItemsType>
            ENGINE_STACK_CLASS_NAME::Stack(int InitialCapacity)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                Items = new ResizableArray<ItemsType, false>(InitialCapacity);
                Count = 0;
            }

            template <typename ItemsType>
            ENGINE_STACK_CLASS_NAME::~Stack()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                delete Items;
            }

            template <typename ItemsType>
            void ENGINE_STACK_CLASS_NAME::Push(ItemsType Item)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                while (Count >= Items->GetLength())
                    if (Items->GetLength() > 0)
                        Items->Resize(Items->GetLength() * 2);
                    else
                        Items->Resize(1);

                Items->SetItem(Count, Item);
                Count++;
            }

            template <typename ItemsType>
            ItemsType ENGINE_STACK_CLASS_NAME::Pop()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Count <= 0)
                    throw std::logic_error("Cannot pop from an empty stack.");

                Count--;
                ItemsType Item = Items->GetItem(Count);

                if (Count < Items->GetLength() / 2)
                    Items->Resize(Items->GetLength() / 2);

                return Item;
            }

            template <typename ItemsType>
            bool ENGINE_STACK_CLASS_NAME::Pop(ItemsType& ItemOut)
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

            template <typename ItemsType>
            void ENGINE_STACK_CLASS_NAME::SetTop(ItemsType Value)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Count <= 0)
                    throw std::logic_error("Cannot set the top element of an empty stack.");

                Items->SetItem(Count - 1, Value);
            }

            template <typename ItemsType>
            void ENGINE_STACK_CLASS_NAME::Clear()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                Count = 0;
                Items->Resize(0);
            }

            template <typename ItemsType>
            void ENGINE_STACK_CLASS_NAME::Expand(int Space)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Space < 0)
                    throw std::domain_error("Space is less than zero.");

                Items->Resize(Items->GetLength() + Space);
            }

            template <typename ItemsType>
            void ENGINE_STACK_CLASS_NAME::Shrink(int AdditionalSpace)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (AdditionalSpace < 0)
                    throw std::domain_error("AdditionalSpace is less than zero.");

                Items->Resize(Count + AdditionalSpace);
            }

            template <typename ItemsType>
            ItemsType ENGINE_STACK_CLASS_NAME::GetTop()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                if (Count <= 0)
                    throw std::logic_error("Cannot get the top element of an empty stack.");

                return Items->GetItem(Count - 1);
            }

            template <typename ItemsType>
            int ENGINE_STACK_CLASS_NAME::GetDepthOf(ItemsType Item)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = Count - 1; i >= 0; i--)
                    if (Items->GetItem(i) == Item)
                        return Count - i;

                return -1;
            }

            template <typename ItemsType>
            bool ENGINE_STACK_CLASS_NAME::Exists(ItemsType Item)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = Count - 1; i >= 0; i--)
                    if (Items->GetItem(i) == Item)
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

                return Items->GetLength();
            }
        }
    }
}

#undef ENGINE_COLLECTION_WRITE_ACCESS
#undef ENGINE_COLLECTION_READ_ACCESS

#undef ENGINE_STACK_CLASS_NAME

#ifndef ENGINE_STACK_USE_MUTEX
    #define ENGINE_STACK_USE_MUTEX
    #include "Stack.h"
    #undef ENGINE_STACK_USE_MUTEX
#endif

#endif // Include Guard