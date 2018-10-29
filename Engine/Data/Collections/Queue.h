#ifndef ENGINE_QUEUE_INCLUDED

#ifdef ENGINE_QUEUE_USE_MUTEX
    #define ENGINE_QUEUE_INCLUDED
#endif

#include "../../Engine.dec.h"
#include "ResizableArray.h"

#ifdef ENGINE_QUEUE_USE_MUTEX
    #include "../HandledMutex.h"
    #define ENGINE_QUEUE_CLASS_NAME Queue<ItemsType, true>
#else
    #define ENGINE_QUEUE_CLASS_NAME Queue<ItemsType, false>
#endif

namespace Engine
{
    namespace Data
    {
        namespace Collections
        {
            template <typename ItemsType>
            class ENGINE_QUEUE_CLASS_NAME
            {
            public:
                Queue(const Queue&) = delete;
                Queue& operator=(const Queue&) = delete;

                Queue(int InitialCapacity = 0);
                ~Queue();

                void Push(ItemsType Item);
                ItemsType Pop();
                bool Pop(ItemsType& ItemOut);
                void Clear();

                void Expand(int Space);
                void Shrink(int AdditionalSpace = 0);

                ItemsType GetFirst();
                int GetDepthOf(ItemsType Item);
                bool Exists(ItemsType Item);
                int GetCount();
                bool IsEmpty();
                int GetCapacity();
            private:
#ifdef ENGINE_QUEUE_USE_MUTEX
                HandledMutex Mutex;
#endif
                int First;
                int Count;
                ResizableArray<ItemsType, false> * Items;

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
    namespace Data
    {
        namespace Collections
        {
            template <typename ItemsType>
            ENGINE_QUEUE_CLASS_NAME::Queue(int InitialCapacity)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                Items = new ResizableArray<ItemsType, false>(InitialCapacity);
                First = 0;
                Count = 0;
            }

            template <typename ItemsType>
            ENGINE_QUEUE_CLASS_NAME::~Queue()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                delete Items;
            }

            template <typename ItemsType>
            void ENGINE_QUEUE_CLASS_NAME::Push(ItemsType Item)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                while (Count >= Items->GetLength())
                    if (Items->GetLength() > 0)
                        Resize(Items->GetLength() * 2);
                    else
                        Resize(1);

                Items->SetItem((First + Count) % Items->GetLength(), Item);
                Count++;
            }

            template <typename ItemsType>
            ItemsType ENGINE_QUEUE_CLASS_NAME::Pop()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Count <= 0)
                    throw std::logic_error("Cannot pop from an empty queue.");

                ItemsType Item = Items->GetItem(First);
                First = (First + 1) % Items->GetLength();
                Count--;

                if (Count < Items->GetLength() / 2)
                    Resize(Items->GetLength() / 2);

                return Item;
            }

            template <typename ItemsType>
            bool ENGINE_QUEUE_CLASS_NAME::Pop(ItemsType& ItemOut)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Count <= 0)
                    return false;

                ItemsType ItemOut = Items->GetItem(First);
                First = (First + 1) % Items->GetLength();
                Count--;

                if (Count < Items->GetLength() / 2)
                    Resize(Items->GetLength() / 2);

                return true;
            }

            template <typename ItemsType>
            void ENGINE_QUEUE_CLASS_NAME::Clear()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                First = 0;
                Count = 0;
                Items->Resize(0);
            }

            template <typename ItemsType>
            void ENGINE_QUEUE_CLASS_NAME::Expand(int Space)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Space < 0)
                    throw std::domain_error("Space is less than zero.");

                Resize(Items->GetLength() + Space);
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
            ItemsType ENGINE_QUEUE_CLASS_NAME::GetFirst()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                if (Count <= 0)
                    throw std::logic_error("Cannot get the first element of an empty queue.");

                return Items->GetItem(First);
            }

            template <typename ItemsType>
            int ENGINE_QUEUE_CLASS_NAME::GetDepthOf(ItemsType Item)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                if (Count == 0)
                    return -1;

                int Last = (First + Count - 1) % Items->GetLength();

                if (First <= Last)
                {
                    for (int i = First; i <= Last; i++)
                        if (Items->GetItem(i) == Item)
                            return i - First;
                }
                else
                {
                    for (int i = First; i < Items->GetLength(); i++)
                        if (Items->GetItem(i) == Item)
                            return i - First;
                    for (int i = 0; i <= Last; i++)
                        if (Items->GetItem(i) == Item)
                            return (Items->GetLength() - First) + i;
                }

                return -1;
            }

            template <typename ItemsType>
            bool ENGINE_QUEUE_CLASS_NAME::Exists(ItemsType Item)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                if (Count == 0)
                    return false;

                int Last = (First + Count - 1) % Items->GetLength();

                if (First <= Last)
                {
                    for (int i = First; i <= Last; i++)
                        if (Items->GetItem(i) == Item)
                            return true;
                }
                else
                {
                    for (int i = First; i < Items->GetLength(); i++)
                        if (Items->GetItem(i) == Item)
                            return true;
                    for (int i = 0; i <= Last; i++)
                        if (Items->GetItem(i) == Item)
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

                return Items->GetLength();
            }

            template <typename ItemsType>
            void ENGINE_QUEUE_CLASS_NAME::Resize(int NewCapacity)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (First == 0 || Count == 0)
                    Items->Resize(NewCapacity);
                else
                {
                    ResizableArray<ItemsType, false> * PrevItems = Items;
                    int prev_capacity = PrevItems->GetLength();
                    Items = new ResizableArray<ItemsType, false>(NewCapacity);
                    for (int i = 0; i < Count; i++)
                        Items->SetItem(i, PrevItems->GetItem((First + i) % prev_capacity));
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

#ifndef ENGINE_QUEUE_USE_MUTEX
    #define ENGINE_QUEUE_USE_MUTEX
    #include "Queue.h"
    #undef ENGINE_QUEUE_USE_MUTEX
#endif

#endif // Include Guard