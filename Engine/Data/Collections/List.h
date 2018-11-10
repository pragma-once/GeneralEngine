#ifndef ENGINE_LIST_INCLUDED

#ifdef ENGINE_LIST_USE_MUTEX
    #define ENGINE_LIST_INCLUDED
#endif

#include "../../Engine.dec.h"
#include "ResizableArray.h"

#ifdef ENGINE_LIST_USE_MUTEX
    #include "../HandledMutex.h"
    #define ENGINE_LIST_CLASS_NAME List<ItemsType, true>
#else
    #define ENGINE_LIST_CLASS_NAME List<ItemsType, false>
#endif

namespace Engine
{
    namespace Data
    {
        namespace Collections
        {
            template <typename ItemsType>
            class ENGINE_LIST_CLASS_NAME // TODO: change expansion and freeing space in all collections and add Mutex locking to all the collections
            {
            public:
                typedef std::function<bool(ENGINE_LIST_CLASS_NAME * Parent, ItemsType& Item, int& Index)> OnAddCallback;
                typedef std::function<bool(ENGINE_LIST_CLASS_NAME * Parent, int& Index, ItemsType& Value)> OnSetItemCallback;
                typedef std::function<bool(ENGINE_LIST_CLASS_NAME * Parent, int& Index)> OnRemoveCallback;
                typedef std::function<bool(ENGINE_LIST_CLASS_NAME * Parent)> OnClearCallback;
                typedef std::function<bool(const ItemsType Item)> Predicate;
                typedef std::function<void(ItemsType Item)> ForEachBody;
                typedef std::function<void(ItemsType Item, bool& BreakLoop)> ForEachBodyWithBreakBool;
                typedef std::function<void(ItemsType Item, std::function<void()> Break)> ForEachBodyWithBreakFunction;

                List(const List&) = delete;
                List& operator=(const List&) = delete;

                List(int InitialCapacity = 0);
                List(
                    OnAddCallback OnAdd,
                    OnSetItemCallback OnSetItem,
                    OnRemoveCallback OnRemove,
                    OnClearCallback OnClear
                );
                ~List();

                ENGINE_LIST_CLASS_NAME * GetChild(
                    OnAddCallback OnAdd,
                    OnSetItemCallback OnSetItem,
                    OnRemoveCallback OnRemove,
                    OnClearCallback OnClear
                );

                bool Add(ItemsType Item);
                bool Add(ItemsType Item, int Index);
                bool SetItem(int Index, ItemsType Value);
                bool Remove(ItemsType Item);
                bool RemoveByIndex(int Index);
                bool Clear();

                void Expand(int Space);
                void Shrink(int AdditionalSpace = 0);

                ItemsType GetItem(int Index);
                int Find(ItemsType Item, int FromIndex = 0);
                bool Contains(ItemsType Item);
                int Find(Predicate, int FromIndex = 0);
                bool Contains(Predicate);
                int GetCount();
                int GetCapacity();
                void ForEach(ForEachBody Body);
                void ForEach(ForEachBodyWithBreakBool Body);
                void ForEach(ForEachBodyWithBreakFunction Body);
            private:
                class LoopBreaker {};

#ifdef ENGINE_LIST_USE_MUTEX
                HandledMutex Mutex;
#endif
                ResizableArray<ItemsType, false> * Items;
                int * CountRef;

                ENGINE_LIST_CLASS_NAME * Parent;
                ResizableArray<ENGINE_LIST_CLASS_NAME*, false> * Children; // Objects to destruct when destructed
                bool IsParentDestructed;

                OnAddCallback OnAdd;
                OnRemoveCallback OnRemove;
                OnClearCallback OnClear;
                OnSetItemCallback OnSetItem;

                List(
                    ENGINE_LIST_CLASS_NAME * Parent,
                    OnAddCallback OnAdd,
                    OnRemoveCallback OnRemove,
                    OnSetItemCallback OnSetItem,
                    OnClearCallback OnClear);
                void DestructChildren();
            };
        }
    }
}

// DEFINITION ----------------------------------------------------------------

#ifdef ENGINE_LIST_USE_MUTEX
    #define ENGINE_COLLECTION_WRITE_MEMBERS_ACCESS auto guard = Mutex.GetLock();
    #define ENGINE_COLLECTION_WRITE_ACCESS auto guard = Mutex.GetLock(); if (IsParentDestructed) throw std::exception("The parent is destructed!");
    #define ENGINE_COLLECTION_READ_ACCESS auto guard = Mutex.GetSharedLock(); if (IsParentDestructed) throw std::exception("The parent is destructed!");
#else
    #define ENGINE_COLLECTION_WRITE_MEMBERS_ACCESS ;
    #define ENGINE_COLLECTION_WRITE_ACCESS if (IsParentDestructed) throw std::exception("The parent is destructed!");
    #define ENGINE_COLLECTION_READ_ACCESS if (IsParentDestructed) throw std::exception("The parent is destructed!");
#endif

namespace Engine
{
    namespace Data
    {
        namespace Collections
        {
            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME::List(int InitialCapacity)
            {
                ENGINE_COLLECTION_WRITE_MEMBERS_ACCESS;

                Parent = nullptr;
                Children = new ResizableArray<ENGINE_LIST_CLASS_NAME*, false>();
                IsParentDestructed = false;

                Items = new ResizableArray<ItemsType, false>(InitialCapacity);
                CountRef = new int(0);

                OnAdd = [this](ENGINE_LIST_CLASS_NAME * Parent, ItemsType& Item, int& Index) -> bool {
                    if (Index > *CountRef || Index < 0)
                        throw std::out_of_range("Index is out of range.");

                    while (*CountRef >= Items->GetLength())
                        if (Items->GetLength() > 0)
                            Items->Resize(Items->GetLength() * 2);
                        else
                            Items->Resize(1);

                    for (int i = *CountRef; i > Index; i--)
                        Items->SetItem(i, Items->GetItem(i - 1));
                    Items->SetItem(Index, Item);

                    (*CountRef)++;

                    return true;
                };

                OnSetItem = [this](ENGINE_LIST_CLASS_NAME * Parent, int& Index, ItemsType& Value) -> bool {
                    if (Index >= *CountRef || Index < 0)
                        throw std::out_of_range("Index is out of range.");

                    Items->SetItem(Index, Value);
                    return true;
                };

                OnRemove = [this](ENGINE_LIST_CLASS_NAME * Parent, int& Index) -> bool {
                    if (Index >= *CountRef || Index < 0)
                        throw std::out_of_range("Index is out of range.");

                    for (int i = Index + 1; i < *CountRef; i++)
                        Items->SetItem(i - 1, Items->GetItem(i));
                    (*CountRef)--;

                    if (*CountRef < Items->GetLength() / 2)
                        Items->Resize(Items->GetLength() / 2);

                    return true;
                };

                OnClear = [this](ENGINE_LIST_CLASS_NAME * Parent) -> bool {
                    *CountRef = 0;
                    Items->Resize(0);
                    return true;
                };
            }

            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME::List(
                OnAddCallback OnAdd,
                OnSetItemCallback OnSetItem,
                OnRemoveCallback OnRemove,
                OnClearCallback OnClear)
            {
                ENGINE_COLLECTION_WRITE_MEMBERS_ACCESS;

                if (OnAdd == nullptr)
                    OnAdd = [](ENGINE_LIST_CLASS_NAME*, ItemsType, int) -> bool { return false; };
                if (OnRemove == nullptr)
                    OnRemove = [](ENGINE_LIST_CLASS_NAME*, int) -> bool { return false; };
                if (OnSetItem == nullptr)
                    OnSetItem = [](ENGINE_LIST_CLASS_NAME*, int, ItemsType) -> bool { return false; };
                if (OnClear == nullptr)
                    OnClear = [](ENGINE_LIST_CLASS_NAME*) -> bool { return false; };

                Parent = new ENGINE_LIST_CLASS_NAME();
                Children = new ResizableArray<ENGINE_LIST_CLASS_NAME*, false>(1);
                Children->SetItem(0, Parent); // To destruct the parent when destructed

                Items = Parent->Items;
                CountRef = Parent->CountRef;

                IsParentDestructed = false;

                this->OnAdd = OnAdd;
                this->OnRemove = OnRemove;
                this->OnSetItem = OnSetItem;
                this->OnClear = OnClear;
            }

            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME::~List()
            {
                ENGINE_COLLECTION_WRITE_MEMBERS_ACCESS;

                if (Parent != nullptr) for (int i = 0; i < Parent->Children->GetLength(); i++) if (Parent->Children->GetItem(i) == this)
                {
                    for (int j = i; j < Parent->Children->GetLength(); j++)
                        Parent->Children->SetItem(j, Parent->Children->GetItem(j + 1));
                    Parent->Children->Resize(Parent->Children->GetLength() - 1);
                }
                DestructChildren();
                if (Parent == nullptr)
                {
                    delete Items;
                    delete CountRef;
                }
                delete Children;
            }

            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME * ENGINE_LIST_CLASS_NAME::GetChild(
                OnAddCallback OnAdd,
                OnSetItemCallback OnSetItem,
                OnRemoveCallback OnRemove,
                OnClearCallback OnClear)
            {
                ENGINE_COLLECTION_WRITE_MEMBERS_ACCESS;

                if (OnAdd == nullptr)
                    OnAdd = [](ENGINE_LIST_CLASS_NAME*, ItemsType, int) -> bool { return false; };
                if (OnRemove == nullptr)
                    OnRemove = [](ENGINE_LIST_CLASS_NAME*, int) -> bool { return false; };
                if (OnSetItem == nullptr)
                    OnSetItem = [](ENGINE_LIST_CLASS_NAME*, int, ItemsType) -> bool { return false; };
                if (OnClear == nullptr)
                    OnClear = [](ENGINE_LIST_CLASS_NAME*) -> bool { return false; };

                ENGINE_LIST_CLASS_NAME * Child = new ENGINE_LIST_CLASS_NAME(this, OnAdd, OnRemove, OnSetItem, OnClear);

                Children->Resize(Children->GetLength() + 1);
                Children->SetItem(Children->GetLength() - 1, Child);

                return Child;
            }



            template <typename ItemsType>
            bool ENGINE_LIST_CLASS_NAME::Add(ItemsType Item)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                return OnAdd(Parent, Item, *CountRef);
            }

            template <typename ItemsType>
            bool ENGINE_LIST_CLASS_NAME::Add(ItemsType Item, int Index)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                return OnAdd(Parent, Item, Index);
            }

            template <typename ItemsType>
            bool ENGINE_LIST_CLASS_NAME::SetItem(int Index, ItemsType Value)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                return OnSetItem(Parent, Index, Value);
            }

            template <typename ItemsType>
            bool ENGINE_LIST_CLASS_NAME::Remove(ItemsType Item)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                for (int i = 0; i < *CountRef; i++)
                    if (Items->GetItem(i) == Item)
                        return OnRemove(Parent, i);
                return false;
            }

            template <typename ItemsType>
            bool ENGINE_LIST_CLASS_NAME::RemoveByIndex(int Index)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                return OnRemove(Parent, Index);
            }

            template <typename ItemsType>
            bool ENGINE_LIST_CLASS_NAME::Clear()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                return OnClear(Parent);
            }



            template <typename ItemsType>
            void ENGINE_LIST_CLASS_NAME::Expand(int Space)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Space >= 0)
                    Items->Resize(Items->GetLength() + Space);
                else
                    throw std::domain_error("Space is less than zero.");
            }

            template <typename ItemsType>
            void ENGINE_LIST_CLASS_NAME::Shrink(int AdditionalSpace)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (AdditionalSpace >= 0)
                    Items->Resize(*CountRef + AdditionalSpace);
                else
                    throw std::domain_error("AdditionalSpace is less than zero.");
            }



            template <typename ItemsType>
            ItemsType ENGINE_LIST_CLASS_NAME::GetItem(int Index)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                if (Index >= *CountRef || Index < 0)
                    throw std::out_of_range("Index is out of range.");

                return Items->GetItem(Index);
            }

            template <typename ItemsType>
            int ENGINE_LIST_CLASS_NAME::Find(ItemsType Item, int FromIndex)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = FromIndex; i < *CountRef; i++)
                    if (Items->GetItem(i) == Item)
                        return i;
                return -1;
            }

            template <typename ItemsType>
            bool ENGINE_LIST_CLASS_NAME::Contains(ItemsType Item)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = 0; i < *CountRef; i++)
                    if (Items->GetItem(i) == Item)
                        return true;
                return false;
            }

            template <typename ItemsType>
            int ENGINE_LIST_CLASS_NAME::Find(Predicate P, int FromIndex)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = FromIndex; i < *CountRef; i++)
                    if (P(Items->GetItem(i)))
                        return i;
                return -1;
            }

            template <typename ItemsType>
            bool ENGINE_LIST_CLASS_NAME::Contains(Predicate P)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = 0; i < *CountRef; i++)
                    if (P(Items->GetItem(i)))
                        return true;
                return false;
            }

            template <typename ItemsType>
            int ENGINE_LIST_CLASS_NAME::GetCount()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return *CountRef;
            }

            template <typename ItemsType>
            int ENGINE_LIST_CLASS_NAME::GetCapacity()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return Items->GetLength();
            }

            template <typename ItemsType>
            void ENGINE_LIST_CLASS_NAME::ForEach(ForEachBody Body)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = 0; i < *CountRef; i++)
                    Body(Items->GetItem(i));
            }

            template <typename ItemsType>
            void ENGINE_LIST_CLASS_NAME::ForEach(ForEachBodyWithBreakBool Body)
            {
                ENGINE_COLLECTION_READ_ACCESS;
                
                bool Break = false;
                for (int i = 0; i < *CountRef; i++)
                {
                    Body(Items->GetItem(i), Break);
                    if (Break) break;
                }
            }

            template <typename ItemsType>
            void ENGINE_LIST_CLASS_NAME::ForEach(ForEachBodyWithBreakFunction Body)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                std::function<void()> BreakFunction = []() { throw LoopBreaker() };
                for (int i = 0; i < *CountRef; i++) try
                {
                    Body(Items->GetItem(i), BreakFunction);
                }
                catch (LoopBreaker&) { break; }
            }



            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME::List(
                ENGINE_LIST_CLASS_NAME * Parent,
                OnAddCallback OnAdd,
                OnRemoveCallback OnRemove,
                OnSetItemCallback OnSetItem,
                OnClearCallback OnClear)
            {
                ENGINE_COLLECTION_WRITE_MEMBERS_ACCESS

                this->Parent = Parent;
                Children = new ResizableArray<ENGINE_LIST_CLASS_NAME*, false>(0);

                Items = Parent->Items;
                CountRef = Parent->CountRef;

                IsParentDestructed = false;

                this->OnAdd = OnAdd;
                this->OnRemove = OnRemove;
                this->OnSetItem = OnSetItem;
                this->OnClear = OnClear;
            }

            template <typename ItemsType>
            void ENGINE_LIST_CLASS_NAME::DestructChildren()
            {
                ENGINE_COLLECTION_WRITE_MEMBERS_ACCESS

                for (int i = 0; i < Children->GetLength(); i++)
                    if (Children->GetItem(i)->Parent == nullptr)
                        delete Children->GetItem(i);
                    else
                        Children->GetItem(i)->DestructChildren();
                IsParentDestructed = true;
            }
        }
    }
}

#undef ENGINE_COLLECTION_WRITE_MEMBERS_ACCESS
#undef ENGINE_COLLECTION_WRITE_ACCESS
#undef ENGINE_COLLECTION_READ_ACCESS

#undef ENGINE_LIST_CLASS_NAME

#ifndef ENGINE_LIST_USE_MUTEX
    #define ENGINE_LIST_USE_MUTEX
    #include "List.h"
    #undef ENGINE_LIST_USE_MUTEX
#endif

#endif // Include Guard