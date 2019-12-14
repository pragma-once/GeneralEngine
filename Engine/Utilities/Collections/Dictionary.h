#ifndef ENGINE_DICTIONARY_INCLUDED

#ifdef ENGINE_DICTIONARY_USE_MUTEX
    #define ENGINE_DICTIONARY_INCLUDED
#endif

#include "../../Engine.dec.h"
#include "ResizableArray.h"

#ifdef ENGINE_DICTIONARY_USE_MUTEX
    #include "../MutexContained.h"
    #define ENGINE_DICTIONARY_CLASS_NAME Dictionary<KeyType, ValueType, true>
    #define ENGINE_DICTIONARY_DERIVATION : public MutexContained<true, false>
#else
    #define ENGINE_DICTIONARY_CLASS_NAME Dictionary<KeyType, ValueType, false>
    #define ENGINE_DICTIONARY_DERIVATION
#endif

namespace Engine
{
    namespace Utilities
    {
        namespace Collections
        {
            // TODO: Switch to a tree structure (low priority)
            template <typename KeyType, typename ValueType>
            class ENGINE_DICTIONARY_CLASS_NAME final ENGINE_DICTIONARY_DERIVATION
            {
#ifdef ENGINE_DICTIONARY_USE_MUTEX
                friend Dictionary<KeyType, ValueType, false>;
#else
                friend Dictionary<KeyType, ValueType, true>;
#endif
            public:
                typedef std::function<void(KeyType Key)> ForEachBody;
                typedef std::function<void(KeyType Key, bool& BreakLoop)> ForEachBodyWithBreakBool;
                typedef std::function<void(KeyType Key, std::function<void()> Break)> ForEachBodyWithBreakFunction;
                typedef std::function<void(KeyType Key, ValueType Value)> ForEachBodyWithValue;
                typedef std::function<void(KeyType Key, ValueType Value, bool& BreakLoop)> ForEachBodyWithValueWithBreakBool;
                typedef std::function<void(KeyType Key, ValueType Value, std::function<void()> Break)> ForEachBodyWithValueWithBreakFunction;

                Dictionary();
                ~Dictionary();

                Dictionary(Dictionary<KeyType, ValueType, true>&) noexcept;
                Dictionary(Dictionary<KeyType, ValueType, true>&&) noexcept;
                Dictionary(Dictionary<KeyType, ValueType, false>&) noexcept;
                Dictionary(Dictionary<KeyType, ValueType, false>&&) noexcept;

                Dictionary& operator=(Dictionary<KeyType, ValueType, true>) noexcept;
                Dictionary& operator=(Dictionary<KeyType, ValueType, false>) noexcept;

                /// @brief Assigns a value to a key.
                void SetValue(KeyType Key, ValueType Value);
                /// @brief Removes a key-value pair.
                void Remove(KeyType Key);
                /// @brief Clears the key-value pairs.
                void Clear();

                /// @brief Gets the value that is assigned to a key.
                ValueType GetValue(KeyType Key);
                /// @brief Checks if a key exists in the dictionary.
                /// @param Key The search subject.
                /// @return True if the key exists in the dictionary.
                bool Contains(KeyType Key);
                /// @brief Gets the items count.
                int GetCount();
                /// @brief Checks whether the dictionary is empty.
                bool IsEmpty();

                /// @brief Calls a function for each key.
                /// @param Body The foreach body function, can be a lambda.
                void ForEach(ForEachBody Body);
                /// @brief Calls a function for each key.
                /// @param Body The foreach body function, can be a lambda.
                ///        Set BreakLoop boolean provided by the parameters to true to break the loop.
                void ForEach(ForEachBodyWithBreakBool Body);
                /// @brief Calls a function for each key.
                /// @param Body The foreach body function, can be a lambda.
                ///        Call Break function provided by the parameters to break the loop.
                void ForEach(ForEachBodyWithBreakFunction Body);
                /// @brief Calls a function for each key-value.
                /// @param Body The foreach body function, can be a lambda.
                void ForEach(ForEachBodyWithValue Body);
                /// @brief Calls a function for each key-value.
                /// @param Body The foreach body function, can be a lambda.
                ///        Set BreakLoop boolean provided by the parameters to true to break the loop.
                void ForEach(ForEachBodyWithValueWithBreakBool Body);
                /// @brief Calls a function for each key-value.
                /// @param Body The foreach body function, can be a lambda.
                ///        Call Break function provided by the parameters to break the loop.
                void ForEach(ForEachBodyWithValueWithBreakFunction Body);
            private:
                class LoopBreaker {};

                int Count;
                ResizableArray<std::pair<KeyType, ValueType>, false> * PairsRef;

                inline int Find(KeyType Key);
            };
        }
    }
}

// DEFINITION ----------------------------------------------------------------

#ifdef ENGINE_DICTIONARY_USE_MUTEX
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
            template <typename KeyType, typename ValueType>
            ENGINE_DICTIONARY_CLASS_NAME::Dictionary()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                PairsRef = new ResizableArray<std::pair<KeyType, ValueType>, false>();
                Count = 0;
            }

            template <typename KeyType, typename ValueType>
            ENGINE_DICTIONARY_CLASS_NAME::~Dictionary()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                delete PairsRef;
            }

            template <typename KeyType, typename ValueType>
            ENGINE_DICTIONARY_CLASS_NAME::Dictionary(Dictionary<KeyType, ValueType, true>& Op) noexcept : Dictionary()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                auto OpGuard = Op.Mutex.GetSharedLock();

                Count = Op.Count;
                *PairsRef = *(Op.PairsRef);
            }

            template <typename KeyType, typename ValueType>
            ENGINE_DICTIONARY_CLASS_NAME::Dictionary(Dictionary<KeyType, ValueType, true>&& Op) noexcept : Dictionary()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                auto OpGuard = Op.Mutex.GetSharedLock();

                std::swap(Count, Op.Count);
                std::swap(PairsRef, Op.PairsRef);
            }

            template <typename KeyType, typename ValueType>
            ENGINE_DICTIONARY_CLASS_NAME::Dictionary(Dictionary<KeyType, ValueType, false>& Op) noexcept : Dictionary()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                Count = Op.Count;
                *PairsRef = *(Op.PairsRef);
            }

            template <typename KeyType, typename ValueType>
            ENGINE_DICTIONARY_CLASS_NAME::Dictionary(Dictionary<KeyType, ValueType, false>&& Op) noexcept : Dictionary()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                std::swap(Count, Op.Count);
                std::swap(PairsRef, Op.PairsRef);
            }

            template <typename KeyType, typename ValueType>
            ENGINE_DICTIONARY_CLASS_NAME& ENGINE_DICTIONARY_CLASS_NAME::operator=(Dictionary<KeyType, ValueType, true> Op) noexcept
            {
                ENGINE_COLLECTION_WRITE_ACCESS;
                auto OpGuard = Op.Mutex.GetSharedLock();

                std::swap(Count, Op.Count);
                std::swap(PairsRef, Op.PairsRef);

                return *this;
            }

            template <typename KeyType, typename ValueType>
            ENGINE_DICTIONARY_CLASS_NAME& ENGINE_DICTIONARY_CLASS_NAME::operator=(Dictionary<KeyType, ValueType, false> Op) noexcept
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                std::swap(Count, Op.Count);
                std::swap(PairsRef, Op.PairsRef);

                return *this;
            }

            template <typename KeyType, typename ValueType>
            void ENGINE_DICTIONARY_CLASS_NAME::SetValue(KeyType Key, ValueType Value)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                int s = 0;
                int e = Count;
                while (s < e)
                {
                    int c = (s + e) / 2;
                    if (Key == PairsRef->GetItem(c).first) { s = c; break; }
                    else if (Key < PairsRef->GetItem(c).first) e = c;
                    else s = c + 1;
                }

                if (Count == 0 || s >= Count || PairsRef->GetItem(s).first != Key)
                {
                    while (Count >= PairsRef->GetLength())
                        if (PairsRef->GetLength() > 0)
                            PairsRef->Resize(PairsRef->GetLength() * 2);
                        else
                            PairsRef->Resize(1);

                    for (int i = Count; i > s; i--)
                        PairsRef->SetItem(i, PairsRef->GetItem(i - 1));
                    Count++;
                }

                PairsRef->SetItem(s, std::pair<KeyType, ValueType>(Key, Value));
            }

            template <typename KeyType, typename ValueType>
            void ENGINE_DICTIONARY_CLASS_NAME::Remove(KeyType Key)
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                for (int i = Find(Key) + 1; i < Count; i++)
                    PairsRef->SetItem(i - 1, PairsRef->GetItem(i));
                Count--;

                if (Count < PairsRef->GetLength() / 2)
                    PairsRef->Resize(PairsRef->GetLength() / 2);
            }

            template <typename KeyType, typename ValueType>
            void ENGINE_DICTIONARY_CLASS_NAME::Clear()
            {
                ENGINE_COLLECTION_WRITE_ACCESS;

                Count = 0;
                PairsRef->Resize(0);
            }

            template <typename KeyType, typename ValueType>
            ValueType ENGINE_DICTIONARY_CLASS_NAME::GetValue(KeyType Key)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return PairsRef->GetItem(Find(Key)).second;
            }

            template <typename KeyType, typename ValueType>
            bool ENGINE_DICTIONARY_CLASS_NAME::Contains(KeyType Key)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                if (Count == 0) return false;

                int s = 0;
                int e = Count - 1;
                while (s < e)
                {
                    int c = (s + e) / 2;
                    if (Key == PairsRef->GetItem(c).first) { s = c; break; }
                    else if (Key < PairsRef->GetItem(c).first) e = c - 1;
                    else s = c + 1;
                }

                if (PairsRef->GetItem(s).first != Key)
                    return false;

                return true;
            }

            template <typename KeyType, typename ValueType>
            int ENGINE_DICTIONARY_CLASS_NAME::GetCount()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return Count;
            }

            template <typename KeyType, typename ValueType>
            bool ENGINE_DICTIONARY_CLASS_NAME::IsEmpty()
            {
                ENGINE_COLLECTION_READ_ACCESS;

                return Count == 0;
            }

            template <typename KeyType, typename ValueType>
            void ENGINE_DICTIONARY_CLASS_NAME::ForEach(ForEachBody Body)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = 0; i < Count; i++)
                    Body(PairsRef->GetItem(i).first);
            }

            template <typename KeyType, typename ValueType>
            void ENGINE_DICTIONARY_CLASS_NAME::ForEach(ForEachBodyWithBreakBool Body)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                bool ShouldBreak = false;
                for (int i = 0; i < Count; i++)
                {
                    Body(PairsRef->GetItem(i).first, ShouldBreak);
                    if (ShouldBreak) break;
                }
            }

            template <typename KeyType, typename ValueType>
            void ENGINE_DICTIONARY_CLASS_NAME::ForEach(ForEachBodyWithBreakFunction Body)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                std::function<void()> BreakFunction = []() { throw LoopBreaker(); };
                for (int i = 0; i < Count; i++) try
                {
                    Body(PairsRef->GetItem(i).first, BreakFunction);
                }
                catch (LoopBreaker&) { break; }
            }

            template <typename KeyType, typename ValueType>
            void ENGINE_DICTIONARY_CLASS_NAME::ForEach(ForEachBodyWithValue Body)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                for (int i = 0; i < Count; i++)
                    Body(PairsRef->GetItem(i).first, PairsRef->GetItem(i).second);
            }

            template <typename KeyType, typename ValueType>
            void ENGINE_DICTIONARY_CLASS_NAME::ForEach(ForEachBodyWithValueWithBreakBool Body)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                bool ShouldBreak = false;
                for (int i = 0; i < Count; i++)
                {
                    Body(PairsRef->GetItem(i).first, PairsRef->GetItem(i).second, ShouldBreak);
                    if (ShouldBreak) break;
                }
            }

            template <typename KeyType, typename ValueType>
            void ENGINE_DICTIONARY_CLASS_NAME::ForEach(ForEachBodyWithValueWithBreakFunction Body)
            {
                ENGINE_COLLECTION_READ_ACCESS;

                std::function<void()> BreakFunction = []() { throw LoopBreaker(); };
                for (int i = 0; i < Count; i++) try
                {
                    Body(PairsRef->GetItem(i).first, PairsRef->GetItem(i).second, BreakFunction);
                }
                catch (LoopBreaker&) { break; }
            }

            template <typename KeyType, typename ValueType>
            int ENGINE_DICTIONARY_CLASS_NAME::Find(KeyType Key)
            {
                if (Count == 0)
                    throw std::domain_error("Key not found.");

                int s = 0;
                int e = Count - 1;
                while (s < e)
                {
                    int c = (s + e) / 2;
                    if (Key == PairsRef->GetItem(c).first) { s = c; break; }
                    else if (Key < PairsRef->GetItem(c).first) e = c - 1;
                    else s = c + 1;
                }

                if (PairsRef->GetItem(s).first != Key)
                    throw std::domain_error("Key not found.");

                return s;
            }
        }
    }
}

#undef ENGINE_COLLECTION_WRITE_ACCESS
#undef ENGINE_COLLECTION_READ_ACCESS

#undef ENGINE_DICTIONARY_CLASS_NAME
#undef ENGINE_DICTIONARY_DERIVATION

#ifndef ENGINE_DICTIONARY_USE_MUTEX
    #define ENGINE_DICTIONARY_USE_MUTEX
    #include "Dictionary.h"
    #undef ENGINE_DICTIONARY_USE_MUTEX
#endif

#endif // Include Guard
