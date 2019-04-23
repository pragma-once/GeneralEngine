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
            class ENGINE_LIST_CLASS_NAME final
            {
#ifdef ENGINE_LIST_USE_MUTEX
                friend List<ItemsType, false>;
#else
                friend List<ItemsType, true>;
#endif
            public:
                typedef std::function<void(ENGINE_LIST_CLASS_NAME * Parent, ItemsType& Item, int& Index)> OnAddCallback;
                typedef std::function<void(ENGINE_LIST_CLASS_NAME * Parent, int& Index, ItemsType& Value)> OnSetItemCallback;
                typedef std::function<void(ENGINE_LIST_CLASS_NAME * Parent, int& Index)> OnRemoveCallback;
                typedef std::function<void(ENGINE_LIST_CLASS_NAME * Parent)> OnClearCallback;
                typedef std::function<bool(ItemsType Item)> Predicate;
                typedef std::function<void(ItemsType Item)> ForEachBody;
                typedef std::function<void(ItemsType Item, bool& BreakLoop)> ForEachBodyWithBreakBool;
                typedef std::function<void(ItemsType Item, std::function<void()> Break)> ForEachBodyWithBreakFunction;

                List(int InitialCapacity = 0);
                /// @brief Creates a list and provides a user-defined interface for that list.
                ///
                /// The first parameter of the parameter functions, is a pointer
                /// to the main modifiable list.
                /// It's developer's responsibility to implement the right functions.
                ///
                /// @param OnAdd What to do on add
                /// @param OnSetItem What to do when setting an item
                /// @param OnRemove What to do on remove
                /// @param OnClear What to do on clear
                /// @return The list interface
                List(
                    OnAddCallback OnAdd,
                    OnSetItemCallback OnSetItem,
                    OnRemoveCallback OnRemove,
                    OnClearCallback OnClear
                );
                ~List();

                List(List<ItemsType, true>&);
                List(List<ItemsType, true>&&);
                List(List<ItemsType, false>&);
                List(List<ItemsType, false>&&);

                List& operator=(List<ItemsType, true>&);
                List& operator=(List<ItemsType, true>&&);
                List& operator=(List<ItemsType, false>&);
                List& operator=(List<ItemsType, false>&&);

                List<ItemsType, true> operator+(List<ItemsType, true>&);
                List<ItemsType, true> operator+(List<ItemsType, true>&&);
                List<ItemsType, true> operator+(List<ItemsType, false>&);
                List<ItemsType, true> operator+(List<ItemsType, false>&&);

                List& operator+=(List<ItemsType, true>&);
                List& operator+=(List<ItemsType, true>&&);
                List& operator+=(List<ItemsType, false>&);
                List& operator+=(List<ItemsType, false>&&);

                /// @brief Creates an interface for this list.
                ///
                /// The first parameter of the parameter functions, is a pointer
                /// to this list/interface.
                /// It's developer's responsibility to implement the right functions.
                ///
                /// @param OnAdd What to do on add
                /// @param OnSetItem What to do when setting an item
                /// @param OnRemove What to do on remove
                /// @param OnClear What to do on clear
                /// @return The created list interface
                ENGINE_LIST_CLASS_NAME * CreateInterface(
                    OnAddCallback OnAdd = nullptr,
                    OnSetItemCallback OnSetItem = nullptr,
                    OnRemoveCallback OnRemove = nullptr,
                    OnClearCallback OnClear = nullptr
                );

                /// @brief Adds/appends an item to the end of the list by default.
                ///        Behavior might vary based on the interface that is used to access the list.
                void Add(ItemsType Item);
                /// @brief Inserts an item to the list at a specified index.
                ///        Behavior might vary based on the interface that is used to access the list.
                void Add(ItemsType Item, int Index);
                /// @brief Sets an item at a specified index.
                ///        Behavior might vary based on the interface that is used to access the list.
                void SetItem(int Index, ItemsType Value);
                /// @brief Removes the first found item that is equal to the Item parameter.
                ///        Behavior might vary based on the interface that is used to access the list.
                /// @return Whether an item was found to be removed.
                bool Remove(ItemsType Item);
                /// @brief Removes an item at a specified index.
                ///        Behavior might vary based on the interface that is used to access the list.
                void RemoveByIndex(int Index);
                /// @brief Clears the list's items.
                ///        Behavior might vary based on the interface that is used to access the list.
                void Clear();

                /// @brief Expands the allocated memory.
                /// @param Space the space to add to the allocated memory.
                void Expand(int Space);
                /// @brief Shrinks the allocated memory.
                /// @param AdditionalSpace The space that must be left empty.
                ///        0 will shrink the space to fit the items count.
                void Shrink(int AdditionalSpace = 0);
                /// @brief Sets whether the list must shrink automatically on removing.
                ///
                /// Shrinking can be manually controlled in quick add/remove situations.
                /// It is recommended to leave the AutoShrink on.
                void SetAutoShrink(bool Value);
                /// @brief Gets whether the list shrinks automatically.
                ///
                /// Set the value using SetAutoShrink(bool).
                bool IsAutoShrink();

                /// @brief Gets an item at a specified index.
                ItemsType GetItem(int Index);
                /// @brief Finds the first matching item.
                ///
                /// Searches for the item until the last item and
                /// doesn't return back to search all items if FromIndex > 0.
                ///
                /// @param Item The search subject.
                /// @param FromIndex The start index for searching.
                /// @return The index of the matching item if found,
                ///         -1 if no matching item found.
                int Find(ItemsType Item, int FromIndex = 0);
                /// @brief Checks if a matching item exists in the list.
                /// @param Item The search subject.
                /// @return True if the item was found once, else false.
                bool Contains(ItemsType Item);
                /// @brief Finds the first item that satisfies the Predicate.
                /// @param Predicate Gets an item and returns whether it's the search subject.
                ///        Predicate can be a lambda.
                /// @param FromIndex The start index for searching.
                /// @return The index of the item if found,
                ///         -1 if no item found.
                int Find(Predicate Predicate, int FromIndex = 0);
                /// @brief Checks if an item exists in the list that satisfies the Predicate.
                /// @param Predicate Gets an item and returns whether it's the search subject.
                ///        Predicate can be a lambda.
                /// @return True if the item was found once, else false.
                bool Contains(Predicate Predicate);
                /// @brief Gets the items count.
                int GetCount();
                /// @brief Gets the current capacity of the allocated memory.
                int GetCapacity();
                /// @brief Calls a function for each item.
                /// @param Body The foreach body function, can be a lambda.
                void ForEach(ForEachBody Body);
                /// @brief Calls a function for each item.
                /// @param Body The foreach body function, can be a lambda.
                ///        Set BreakLoop boolean provided by the parameters to true to break the loop.
                void ForEach(ForEachBodyWithBreakBool Body);
                /// @brief Calls a function for each item.
                /// @param Body The foreach body function, can be a lambda.
                ///        Call Break function provided by the parameters to break the loop.
                void ForEach(ForEachBodyWithBreakFunction Body);
            private:
                class LoopBreaker {};

#ifdef ENGINE_LIST_USE_MUTEX
                HandledMutex Mutex;
#endif
                ResizableArray<ItemsType, false> * ItemsRef;
                int * CountRef;
                bool * AutoShrinkRef;

                ENGINE_LIST_CLASS_NAME * Parent;
                ResizableArray<ENGINE_LIST_CLASS_NAME*, false> * Children; // Objects to destruct when destructed
                bool IsParentDestructed;
                const bool IsRoot;

                OnAddCallback OnAdd;
                OnSetItemCallback OnSetItem;
                OnRemoveCallback OnRemove;
                OnClearCallback OnClear;
                
                List(
                    ENGINE_LIST_CLASS_NAME * Parent,
                    OnAddCallback OnAdd,
                    OnSetItemCallback OnSetItem,
                    OnRemoveCallback OnRemove,
                    OnClearCallback OnClear);
                void DestructChildren();
            };
        }
    }
}

// DEFINITION ----------------------------------------------------------------

#ifdef ENGINE_LIST_USE_MUTEX
    #define ENGINE_COLLECTION_STRUCTURE_ACCESS auto guard = Mutex.GetLock();
    #define ENGINE_COLLECTION_WRITE_ACCESS auto guard = Mutex.GetLock(); if (IsParentDestructed) throw std::logic_error("The parent is destructed!");
    #define ENGINE_COLLECTION_READ_ACCESS auto guard = Mutex.GetSharedLock(); if (IsParentDestructed) throw std::logic_error("The parent is destructed!");
#else
    #define ENGINE_COLLECTION_STRUCTURE_ACCESS ;
    #define ENGINE_COLLECTION_WRITE_ACCESS if (IsParentDestructed) throw std::logic_error("The parent is destructed!");
    #define ENGINE_COLLECTION_READ_ACCESS if (IsParentDestructed) throw std::logic_error("The parent is destructed!");
#endif

#define ENGINE_COLLECTION_OPERAND_ACCESS if (Op.IsParentDestructed) throw std::logic_error("The operand's parent is destructed!");
#define ENGINE_COLLECTION_OPERAND_READ_ACCESS_WITH_MUTEX auto OpGuard = Op.Mutex.GetSharedLock(); ENGINE_COLLECTION_OPERAND_ACCESS
#define ENGINE_COLLECTION_OPERAND_WRITE_ACCESS_WITH_MUTEX auto OpGuard = Op.Mutex.GetLock(); ENGINE_COLLECTION_OPERAND_ACCESS

namespace Engine
{
    namespace Data
    {
        namespace Collections
        {
            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME::List(int InitialCapacity) : IsRoot(true)
            {
                ENGINE_COLLECTION_STRUCTURE_ACCESS;

                Parent = nullptr;
                Children = new ResizableArray<ENGINE_LIST_CLASS_NAME*, false>();
                IsParentDestructed = false;

                ItemsRef = new ResizableArray<ItemsType, false>(InitialCapacity);
                CountRef = new int(0);
                AutoShrinkRef = new bool(true);

                OnAdd = [this](ENGINE_LIST_CLASS_NAME * Parent, ItemsType& Item, int& Index) {
                    if (Index > *CountRef || Index < 0)
                        throw std::out_of_range("Index is out of range.");

                    while (*CountRef >= ItemsRef->GetLength())
                        if (ItemsRef->GetLength() > 0)
                            ItemsRef->Resize(ItemsRef->GetLength() * 2);
                        else
                            ItemsRef->Resize(1);

                    for (int i = *CountRef; i > Index; i--)
                        ItemsRef->SetItem(i, ItemsRef->GetItem(i - 1));
                    ItemsRef->SetItem(Index, Item);

                    (*CountRef)++;
                };

                OnSetItem = [this](ENGINE_LIST_CLASS_NAME * Parent, int& Index, ItemsType& Value) {
                    if (Index >= *CountRef || Index < 0)
                        throw std::out_of_range("Index is out of range.");

                    ItemsRef->SetItem(Index, Value);
                };

                OnRemove = [this](ENGINE_LIST_CLASS_NAME * Parent, int& Index) {
                    if (Index >= *CountRef || Index < 0)
                        throw std::out_of_range("Index is out of range.");

                    for (int i = Index + 1; i < *CountRef; i++)
                        ItemsRef->SetItem(i - 1, ItemsRef->GetItem(i));
                    (*CountRef)--;

                    if (*AutoShrinkRef && *CountRef < ItemsRef->GetLength() / 2)
                        ItemsRef->Resize(ItemsRef->GetLength() / 2);
                };

                OnClear = [this](ENGINE_LIST_CLASS_NAME * Parent) {
                    *CountRef = 0;
                    if (*AutoShrinkRef)
                        ItemsRef->Resize(0);
                };
            }

            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME::List(
                OnAddCallback OnAdd,
                OnSetItemCallback OnSetItem,
                OnRemoveCallback OnRemove,
                OnClearCallback OnClear) : IsRoot(false)
            {
                ENGINE_COLLECTION_STRUCTURE_ACCESS;

                if (OnAdd == nullptr)
                    OnAdd = [](ENGINE_LIST_CLASS_NAME*, ItemsType&, int&) { throw std::logic_error("List::Add interface not implemented"); };
                if (OnSetItem == nullptr)
                    OnSetItem = [](ENGINE_LIST_CLASS_NAME*, int&, ItemsType&) { throw std::logic_error("List::SetItem interface not implemented"); };
                if (OnRemove == nullptr)
                    OnRemove = [](ENGINE_LIST_CLASS_NAME*, int&) { throw std::logic_error("List::Remove interface not implemented"); };
                if (OnClear == nullptr)
                    OnClear = [](ENGINE_LIST_CLASS_NAME*) { throw std::logic_error("List::Clear interface not implemented"); };

                Parent = new ENGINE_LIST_CLASS_NAME();
                Children = new ResizableArray<ENGINE_LIST_CLASS_NAME*, false>(1);
                Children->SetItem(0, Parent); // To destruct the parent when destructed

                ItemsRef = Parent->ItemsRef;
                CountRef = Parent->CountRef;
                AutoShrinkRef = Parent->AutoShrinkRef;

                IsParentDestructed = false;

                this->OnAdd = OnAdd;
                this->OnSetItem = OnSetItem;
                this->OnRemove = OnRemove;
                this->OnClear = OnClear;
            }

            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME::~List()
            {
                ENGINE_COLLECTION_STRUCTURE_ACCESS;

                if (Parent != nullptr) for (int i = 0; i < Parent->Children->GetLength(); i++) if (Parent->Children->GetItem(i) == this)
                {
                    for (int j = i; j < Parent->Children->GetLength(); j++)
                        Parent->Children->SetItem(j, Parent->Children->GetItem(j + 1));
                    Parent->Children->Resize(Parent->Children->GetLength() - 1);
                }
                DestructChildren();
                if (Parent == nullptr)
                {
                    delete ItemsRef;
                    delete CountRef;
                    delete AutoShrinkRef;
                }
                delete Children;
            }

            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME::List(List<ItemsType, true>& Op) : List(*(Op.CountRef))
            {
                ENGINE_COLLECTION_STRUCTURE_ACCESS;
                ENGINE_COLLECTION_OPERAND_READ_ACCESS_WITH_MUTEX;

                *CountRef = *(Op.CountRef);
                *ItemsRef = *(Op.ItemsRef);
            }
            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME::List(List<ItemsType, true>&& Op) : List()
            {
                ENGINE_COLLECTION_STRUCTURE_ACCESS;
                ENGINE_COLLECTION_OPERAND_WRITE_ACCESS_WITH_MUTEX;

                std::swap(CountRef, Op.CountRef);
                std::swap(ItemsRef, Op.ItemsRef);
            }
            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME::List(List<ItemsType, false>& Op) : List(*(Op.CountRef))
            {
                ENGINE_COLLECTION_STRUCTURE_ACCESS;
                ENGINE_COLLECTION_OPERAND_ACCESS;

                *CountRef = *(Op.CountRef);
                *ItemsRef = *(Op.ItemsRef);
            }
            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME::List(List<ItemsType, false>&& Op) : List()
            {
                ENGINE_COLLECTION_STRUCTURE_ACCESS;
                ENGINE_COLLECTION_OPERAND_ACCESS;

                std::swap(CountRef, Op.CountRef);
                std::swap(ItemsRef, Op.ItemsRef);
            }


            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME& ENGINE_LIST_CLASS_NAME::operator=(List<ItemsType, true>& Op)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                ENGINE_COLLECTION_OPERAND_READ_ACCESS_WITH_MUTEX;

                if (IsRoot)
                {
                    // Return if the operand is the same list.
                    if (ItemsRef == Op.ItemsRef)
                        return *this;

                    *CountRef = *(Op.CountRef);
                    *ItemsRef = *(Op.ItemsRef);
                }
                else
                {
                    ENGINE_LIST_CLASS_NAME list(OnAdd, OnSetItem, OnRemove, OnClear);
                    Op.ForEach([&list](ItemsType Item) { list.Add(Item); });
                    // No exceptions occured, the list is now complete and ready to swap.
                    std::swap(CountRef, list.CountRef);
                    std::swap(ItemsRef, list.ItemsRef);
                }

                return *this;
            }
            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME& ENGINE_LIST_CLASS_NAME::operator=(List<ItemsType, true>&& Op)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                ENGINE_COLLECTION_OPERAND_WRITE_ACCESS_WITH_MUTEX;

                if (IsRoot)
                {
                    // Return if the operand is the same list.
                    if (ItemsRef == Op.ItemsRef)
                        return *this;

                    delete ItemsRef;
                    *CountRef = std::exchange(*Op.CountRef, 0);
                    ItemsRef  = std::exchange(Op.ItemsRef, new ResizableArray<ItemsType, false>());
                }
                else
                {
                    ENGINE_LIST_CLASS_NAME list(OnAdd, OnSetItem, OnRemove, OnClear);
                    Op.ForEach([&list](ItemsType Item) { list.Add(Item); });
                    // No exceptions occured, the list is now complete and ready to swap.
                    std::swap(CountRef, list.CountRef);
                    std::swap(ItemsRef, list.ItemsRef);
                }

                return *this;
            }
            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME& ENGINE_LIST_CLASS_NAME::operator=(List<ItemsType, false>& Op)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                ENGINE_COLLECTION_OPERAND_ACCESS;

                if (IsRoot)
                {
                    // Return if the operand is the same list.
                    if (ItemsRef == Op.ItemsRef)
                        return *this;

                    *CountRef = *(Op.CountRef);
                    *ItemsRef = *(Op.ItemsRef);
                }
                else
                {
                    ENGINE_LIST_CLASS_NAME list(OnAdd, OnSetItem, OnRemove, OnClear);
                    Op.ForEach([&list](ItemsType Item) { list.Add(Item); });
                    // No exceptions occured, the list is now complete and ready to swap.
                    std::swap(CountRef, list.CountRef);
                    std::swap(ItemsRef, list.ItemsRef);
                }

                return *this;
            }
            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME& ENGINE_LIST_CLASS_NAME::operator=(List<ItemsType, false>&& Op)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                ENGINE_COLLECTION_OPERAND_ACCESS;

                if (IsRoot)
                {
                    // Return if the operand is the same list.
                    if (ItemsRef == Op.ItemsRef)
                        return *this;

                    delete ItemsRef;
                    *CountRef = std::exchange(*Op.CountRef, 0);
                    ItemsRef  = std::exchange(Op.ItemsRef, new ResizableArray<ItemsType, false>());
                }
                else
                {
                    ENGINE_LIST_CLASS_NAME list(OnAdd, OnSetItem, OnRemove, OnClear);
                    Op.ForEach([&list](ItemsType Item) { list.Add(Item); });
                    // No exceptions occured, the list is now complete and ready to swap.
                    std::swap(CountRef, list.CountRef);
                    std::swap(ItemsRef, list.ItemsRef);
                }

                return *this;
            }


            template <typename ItemsType>
            List<ItemsType, true> ENGINE_LIST_CLASS_NAME::operator+(List<ItemsType, true>& Op)
            {
                ENGINE_COLLECTION_READ_ACCESS;
                ENGINE_COLLECTION_OPERAND_READ_ACCESS_WITH_MUTEX;

                List<ItemsType, true> result((*CountRef) + (*Op.CountRef));
                *result.CountRef = (*CountRef) + (*Op.CountRef);

                for (int i = 0; i < *CountRef; i++)
                    result.ItemsRef->SetItem(i, ItemsRef->GetItem(i));
                for (int i = 0; i < *Op.CountRef; i++)
                    result.ItemsRef->SetItem((*CountRef) + i, Op.ItemsRef->GetItem(i));

                return result;
            }
            template <typename ItemsType>
            List<ItemsType, true> ENGINE_LIST_CLASS_NAME::operator+(List<ItemsType, true>&& Op)
            {
                ENGINE_COLLECTION_READ_ACCESS;
                ENGINE_COLLECTION_OPERAND_READ_ACCESS_WITH_MUTEX;

                List<ItemsType, true> result((*CountRef) + (*Op.CountRef));
                *result.CountRef = (*CountRef) + (*Op.CountRef);

                for (int i = 0; i < *CountRef; i++)
                    result.ItemsRef->SetItem(i, ItemsRef->GetItem(i));
                for (int i = 0; i < *Op.CountRef; i++)
                    result.ItemsRef->SetItem((*CountRef) + i, Op.ItemsRef->GetItem(i));

                return result;
            }
            template <typename ItemsType>
            List<ItemsType, true> ENGINE_LIST_CLASS_NAME::operator+(List<ItemsType, false>& Op)
            {
                ENGINE_COLLECTION_READ_ACCESS;
                ENGINE_COLLECTION_OPERAND_ACCESS;

                List<ItemsType, true> result((*CountRef) + (*Op.CountRef));
                *result.CountRef = (*CountRef) + (*Op.CountRef);

                for (int i = 0; i < *CountRef; i++)
                    result.ItemsRef->SetItem(i, ItemsRef->GetItem(i));
                for (int i = 0; i < *Op.CountRef; i++)
                    result.ItemsRef->SetItem((*CountRef) + i, Op.ItemsRef->GetItem(i));

                return result;
            }
            template <typename ItemsType>
            List<ItemsType, true> ENGINE_LIST_CLASS_NAME::operator+(List<ItemsType, false>&& Op)
            {
                ENGINE_COLLECTION_READ_ACCESS;
                ENGINE_COLLECTION_OPERAND_ACCESS;

                List<ItemsType, true> result((*CountRef) + (*Op.CountRef));
                *result.CountRef = (*CountRef) + (*Op.CountRef);

                for (int i = 0; i < *CountRef; i++)
                    result.ItemsRef->SetItem(i, ItemsRef->GetItem(i));
                for (int i = 0; i < *Op.CountRef; i++)
                    result.ItemsRef->SetItem((*CountRef) + i, Op.ItemsRef->GetItem(i));

                return result;
            }


            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME& ENGINE_LIST_CLASS_NAME::operator+=(List<ItemsType, true>& Op)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                ENGINE_COLLECTION_OPERAND_READ_ACCESS_WITH_MUTEX;

                if (IsRoot)
                {
                    if (ItemsRef->GetLength() < (*CountRef) + (*Op.CountRef))
                        ItemsRef->Resize((*CountRef) + (*Op.CountRef));
                    for (int i = 0; i < *Op.CountRef; i++)
                        ItemsRef->SetItem((*CountRef) + i, Op.ItemsRef->GetItem(i));
                    *CountRef += *Op.CountRef;
                }
                else
                {
                    ENGINE_LIST_CLASS_NAME list(OnAdd, OnSetItem, OnRemove, OnClear);
                    *list.CountRef = *CountRef;
                    *list.ItemsRef = *ItemsRef;
                    Op.ForEach([&list](ItemsType Item) { list.Add(Item); });
                    // No exceptions occured, the list is now complete and ready to swap.
                    std::swap(CountRef, list.CountRef);
                    std::swap(ItemsRef, list.ItemsRef);
                }

                return *this;
            }
            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME& ENGINE_LIST_CLASS_NAME::operator+=(List<ItemsType, true>&& Op)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                ENGINE_COLLECTION_OPERAND_READ_ACCESS_WITH_MUTEX;

                if (IsRoot)
                {
                    if (ItemsRef->GetLength() < (*CountRef) + (*Op.CountRef))
                        ItemsRef->Resize((*CountRef) + (*Op.CountRef));
                    for (int i = 0; i < *Op.CountRef; i++)
                        ItemsRef->SetItem((*CountRef) + i, Op.ItemsRef->GetItem(i));
                    *CountRef += *Op.CountRef;
                }
                else
                {
                    ENGINE_LIST_CLASS_NAME list(OnAdd, OnSetItem, OnRemove, OnClear);
                    *list.CountRef = *CountRef;
                    *list.ItemsRef = *ItemsRef;
                    Op.ForEach([&list](ItemsType Item) { list.Add(Item); });
                    // No exceptions occured, the list is now complete and ready to swap.
                    std::swap(CountRef, list.CountRef);
                    std::swap(ItemsRef, list.ItemsRef);
                }

                return *this;
            }
            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME& ENGINE_LIST_CLASS_NAME::operator+=(List<ItemsType, false>& Op)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                ENGINE_COLLECTION_OPERAND_ACCESS;

                if (IsRoot)
                {
                    if (ItemsRef->GetLength() < (*CountRef) + (*Op.CountRef))
                        ItemsRef->Resize((*CountRef) + (*Op.CountRef));
                    for (int i = 0; i < *Op.CountRef; i++)
                        ItemsRef->SetItem((*CountRef) + i, Op.ItemsRef->GetItem(i));
                    *CountRef += *Op.CountRef;
                }
                else
                {
                    ENGINE_LIST_CLASS_NAME list(OnAdd, OnSetItem, OnRemove, OnClear);
                    *list.CountRef = *CountRef;
                    *list.ItemsRef = *ItemsRef;
                    Op.ForEach([&list](ItemsType Item) { list.Add(Item); });
                    // No exceptions occured, the list is now complete and ready to swap.
                    std::swap(CountRef, list.CountRef);
                    std::swap(ItemsRef, list.ItemsRef);
                }

                return *this;
            }
            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME& ENGINE_LIST_CLASS_NAME::operator+=(List<ItemsType, false>&& Op)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                ENGINE_COLLECTION_OPERAND_ACCESS;

                if (IsRoot)
                {
                    if (ItemsRef->GetLength() < (*CountRef) + (*Op.CountRef))
                        ItemsRef->Resize((*CountRef) + (*Op.CountRef));
                    for (int i = 0; i < *Op.CountRef; i++)
                        ItemsRef->SetItem((*CountRef) + i, Op.ItemsRef->GetItem(i));
                    *CountRef += *Op.CountRef;
                }
                else
                {
                    ENGINE_LIST_CLASS_NAME list(OnAdd, OnSetItem, OnRemove, OnClear);
                    *list.CountRef = *CountRef;
                    *list.ItemsRef = *ItemsRef;
                    Op.ForEach([&list](ItemsType Item) { list.Add(Item); });
                    // No exceptions occured, the list is now complete and ready to swap.
                    std::swap(CountRef, list.CountRef);
                    std::swap(ItemsRef, list.ItemsRef);
                }

                return *this;
            }


            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME * ENGINE_LIST_CLASS_NAME::CreateInterface(
                OnAddCallback OnAdd,
                OnSetItemCallback OnSetItem,
                OnRemoveCallback OnRemove,
                OnClearCallback OnClear)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (OnAdd == nullptr)
                    OnAdd = [](ENGINE_LIST_CLASS_NAME*, ItemsType&, int&) { throw std::logic_error("List::Add interface not implemented"); };
                if (OnSetItem == nullptr)
                    OnSetItem = [](ENGINE_LIST_CLASS_NAME*, int&, ItemsType&) { throw std::logic_error("List::SetItem interface not implemented"); };
                if (OnRemove == nullptr)
                    OnRemove = [](ENGINE_LIST_CLASS_NAME*, int&) { throw std::logic_error("List::Remove interface not implemented"); };
                if (OnClear == nullptr)
                    OnClear = [](ENGINE_LIST_CLASS_NAME*) { throw std::logic_error("List::Clear interface not implemented"); };

                ENGINE_LIST_CLASS_NAME * Child = new ENGINE_LIST_CLASS_NAME(this, OnAdd, OnSetItem, OnRemove, OnClear);

                Children->Resize(Children->GetLength() + 1);
                Children->SetItem(Children->GetLength() - 1, Child);

                return Child;
            }



            template <typename ItemsType>
            void ENGINE_LIST_CLASS_NAME::Add(ItemsType Item)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                int Index = *CountRef;
                OnAdd(Parent, Item, Index);
            }

            template <typename ItemsType>
            void ENGINE_LIST_CLASS_NAME::Add(ItemsType Item, int Index)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                OnAdd(Parent, Item, Index);
            }

            template <typename ItemsType>
            void ENGINE_LIST_CLASS_NAME::SetItem(int Index, ItemsType Value)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                OnSetItem(Parent, Index, Value);
            }

            template <typename ItemsType>
            bool ENGINE_LIST_CLASS_NAME::Remove(ItemsType Item)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                for (int i = 0; i < *CountRef; i++)
                    if (ItemsRef->GetItem(i) == Item)
                    {
                        OnRemove(Parent, i);
                        return true;
                    }
                return false;
            }

            template <typename ItemsType>
            void ENGINE_LIST_CLASS_NAME::RemoveByIndex(int Index)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                OnRemove(Parent, Index);
            }

            template <typename ItemsType>
            void ENGINE_LIST_CLASS_NAME::Clear()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                OnClear(Parent);
            }



            template <typename ItemsType>
            void ENGINE_LIST_CLASS_NAME::Expand(int Space)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (Space >= 0)
                    ItemsRef->Resize(ItemsRef->GetLength() + Space);
                else
                    throw std::domain_error("Space is less than zero.");
            }

            template <typename ItemsType>
            void ENGINE_LIST_CLASS_NAME::Shrink(int AdditionalSpace)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                if (AdditionalSpace >= 0)
                    ItemsRef->Resize(*CountRef + AdditionalSpace);
                else
                    throw std::domain_error("AdditionalSpace is less than zero.");
            }

            template <typename ItemsType>
            void ENGINE_LIST_CLASS_NAME::SetAutoShrink(bool Value)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                *AutoShrinkRef = Value;
            }

            template <typename ItemsType>
            bool ENGINE_LIST_CLASS_NAME::IsAutoShrink()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return *AutoShrinkRef;
            }



            template <typename ItemsType>
            ItemsType ENGINE_LIST_CLASS_NAME::GetItem(int Index)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                if (Index >= *CountRef || Index < 0)
                    throw std::out_of_range("Index is out of range.");

                return ItemsRef->GetItem(Index);
            }

            template <typename ItemsType>
            int ENGINE_LIST_CLASS_NAME::Find(ItemsType Item, int FromIndex)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                if (FromIndex < 0)
                    throw std::out_of_range("FromIndex cannot be less than zero.");
                for (int i = FromIndex; i < *CountRef; i++)
                    if (ItemsRef->GetItem(i) == Item)
                        return i;
                return -1;
            }

            template <typename ItemsType>
            bool ENGINE_LIST_CLASS_NAME::Contains(ItemsType Item)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = 0; i < *CountRef; i++)
                    if (ItemsRef->GetItem(i) == Item)
                        return true;
                return false;
            }

            template <typename ItemsType>
            int ENGINE_LIST_CLASS_NAME::Find(Predicate P, int FromIndex)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                if (FromIndex < 0)
                    throw std::out_of_range("FromIndex cannot be less than zero.");
                for (int i = FromIndex; i < *CountRef; i++)
                    if (P(ItemsRef->GetItem(i)))
                        return i;
                return -1;
            }

            template <typename ItemsType>
            bool ENGINE_LIST_CLASS_NAME::Contains(Predicate P)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = 0; i < *CountRef; i++)
                    if (P(ItemsRef->GetItem(i)))
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

                return ItemsRef->GetLength();
            }

            template <typename ItemsType>
            void ENGINE_LIST_CLASS_NAME::ForEach(ForEachBody Body)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = 0; i < *CountRef; i++)
                    Body(ItemsRef->GetItem(i));
            }

            template <typename ItemsType>
            void ENGINE_LIST_CLASS_NAME::ForEach(ForEachBodyWithBreakBool Body)
            {
                ENGINE_COLLECTION_READ_ACCESS;
                
                bool Break = false;
                for (int i = 0; i < *CountRef; i++)
                {
                    Body(ItemsRef->GetItem(i), Break);
                    if (Break) break;
                }
            }

            template <typename ItemsType>
            void ENGINE_LIST_CLASS_NAME::ForEach(ForEachBodyWithBreakFunction Body)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                std::function<void()> BreakFunction = []() { throw LoopBreaker(); };
                for (int i = 0; i < *CountRef; i++) try
                {
                    Body(ItemsRef->GetItem(i), BreakFunction);
                }
                catch (LoopBreaker&) { break; }
            }



            template <typename ItemsType>
            ENGINE_LIST_CLASS_NAME::List(
                ENGINE_LIST_CLASS_NAME * Parent,
                OnAddCallback OnAdd,
                OnSetItemCallback OnSetItem,
                OnRemoveCallback OnRemove,
                OnClearCallback OnClear) : IsRoot(false)
            {
                ENGINE_COLLECTION_STRUCTURE_ACCESS

                this->Parent = Parent;
                Children = new ResizableArray<ENGINE_LIST_CLASS_NAME*, false>(0);

                ItemsRef = Parent->ItemsRef;
                CountRef = Parent->CountRef;
                AutoShrinkRef = Parent->AutoShrinkRef;

                IsParentDestructed = false;

                this->OnAdd = OnAdd;
                this->OnSetItem = OnSetItem;
                this->OnRemove = OnRemove;
                this->OnClear = OnClear;
            }

            template <typename ItemsType>
            void ENGINE_LIST_CLASS_NAME::DestructChildren()
            {
                ENGINE_COLLECTION_STRUCTURE_ACCESS

                for (int i = 0; i < Children->GetLength(); i++)
                    if (Children->GetItem(i)->Parent == nullptr) // If the parent is created by the child using List::List(OnAdd,...)
                        delete Children->GetItem(i);
                    else
                        Children->GetItem(i)->DestructChildren();
                IsParentDestructed = true;
            }
        }
    }
}

#undef ENGINE_COLLECTION_STRUCTURE_ACCESS
#undef ENGINE_COLLECTION_WRITE_ACCESS
#undef ENGINE_COLLECTION_READ_ACCESS

#undef ENGINE_LIST_CLASS_NAME

#ifndef ENGINE_LIST_USE_MUTEX
    #define ENGINE_LIST_USE_MUTEX
    #include "List.h"
    #undef ENGINE_LIST_USE_MUTEX
#endif

#endif // Include Guard
